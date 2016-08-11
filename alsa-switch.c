 /*
 * alsa-switch.c
 *
 * reading from multiple capture device and writing to multiple audio output device 1:1
 * while providing a control channel to mute/unmute certain paths
 *
 * Copyright (c) 2016 Andreas Fink
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

#include "alsa-switch-config.h"
#include "system_util.h"
#ifdef	HAVE_ALSA_ASOUNDLIB_H
#include "alsa_util.h"
#endif


typedef struct sound_pipe
{
	const char *input_device_name;
	const char *output_device_name;
	const char *control_pipe_name;
#ifdef	HAVE_ALSA_ASOUNDLIB_H
	snd_pcm_t *input_handle;
	snd_pcm_t *output_handle;
   	unsigned char *buf;
    ssize_t  bufsize;

#endif
	int controlStream;
	int inStream;
	int outStream;
    int currentPriorityLevel;
    int muted;
	pid_t pid;
} sound_pipe;

void process_command(sound_pipe *pipes,int pipecount, int index, char in);
void mute(sound_pipe *pipe);
void unmute(sound_pipe *pipe);
void setNonBlocking(int fd);


int main(int argc, const char *argv[])
{
	int err;
	int i;
	if(argc < 6)
	{
		fprintf(stderr,"Usage:\n\t%s audio-format sample-rate input-device1 output-device1 control-pipe1 input-device2 output-device2 control-pipe2 ...\n",argv[0]);
		return -1;
	}

	const char *format_string = argv[1];
	const char *rate_string = argv[2];
#ifdef	HAVE_ALSA_ASOUNDLIB_H
    int rate = atoi(rate_string);
	int format = format_string_to_value(format_string);
#endif

	int pipecount = (argc - 3)/3;
	if(((argc -3) % 3) != 0)
	{
		fprintf(stderr,"Usage:\n\t%s input-device1 output-device1 control-pipe1 input-device2 output-device2 control-pipe2 ...\n",argv[0]);
		return -1;
	}
	if((pipecount > 256) || (pipecount < 1))
	{
		fprintf(stderr,"Sorry but zero or less or more than 256 sound pipes is unrealistic\n");
		return -1;
	}

	/* lets make sure we can read/write all sound channels before starting the streams */
	sound_pipe *pipes = calloc(sizeof(sound_pipe),pipecount);	
	for(i=0;i<pipecount;i++)
	{
		sound_pipe *pipe = &pipes[i];
		
		pipe->input_device_name = argv[3+(i*3)+0];
		pipe->output_device_name = argv[3+(i*3)+1];
		pipe->control_pipe_name = argv[3+(i*3)+2];

#ifdef	HAVE_ALSA_ASOUNDLIB_H

#define BUFSIZE (1024 * 4)

        pipe->bufsize   = BUFSIZE*2;
        pipe->buf       = malloc(BUFSIZE * 2);
        memset(pipe->buf, 0, BUFSIZE * 2);

        err = open_sound_device(&pipe->input_handle, pipe->input_device_name, SND_PCM_STREAM_CAPTURE,rate,format,BUFSIZE);
		if(err < 0)
		{
			return err;
		}
		err = open_sound_device(&pipe->output_handle, pipe->output_device_name, SND_PCM_STREAM_PLAYBACK,rate,format,BUFSIZE);
		if(err < 0)
		{
			return err;
        }
        err = snd_pcm_prepare(pipe->output_handle);
		if(err < 0)
		{
			fprintf(stderr, "cannot prepare audio interface '%s' for use(%s)\n",
			 pipe->input_device_name,snd_strerror(err));
			return err;
		}
/*	
		err = snd_pcm_link(pipe->input_handle,pipe->output_handle);
		if(err < 0)
		{
			fprintf(stderr, "cannot link '%s' to '%s': %s\n",
			 pipe->input_device_name,
			 pipe->output_device_name,
			 snd_strerror(err));
			return err;
		}
*/
		err = snd_pcm_start(pipe->input_handle);
		if(err < 0)
		{
			fprintf(stderr, "cannot prepare audio interface '%s' for use(%s)\n",
			 pipe->input_device_name,snd_strerror(err));
			return err;
		}
#endif
		printf("opening control channel %s",pipe->control_pipe_name);
        pipe->controlStream = open(pipe->control_pipe_name,O_RDONLY | O_NONBLOCK);
        setNonBlocking(pipe->controlStream);

		printf(".\n");
		fflush(stdout);
        if(pipe->controlStream < 0)
        {
            /* lets try to create it */
            err = mkfifo(pipe->control_pipe_name, 0666);
            if(err < 0)
            {
                fprintf(stderr,"I can not create %s\n",pipe->control_pipe_name);
                return errno;
            }
            chmod(pipe->control_pipe_name, 0666); /* to get around umask */
            pipe->controlStream = open(pipe->control_pipe_name,O_RDONLY | O_NONBLOCK);
            if(pipe->controlStream < 0)
            {
                fprintf(stderr,"I can not open %s for reading\n",pipe->control_pipe_name);
                return errno;
            }
        }
	}
#ifdef	HAVE_ALSA_ASOUNDLIB_H
	/* we close the sound drivers so the child process can take over */
	for(i=0;i<pipecount;i++)
	{
		sound_pipe *pipe = &pipes[i];
		snd_pcm_close(pipe->output_handle);
		snd_pcm_close(pipe->input_handle);
	}
#endif

	for(i=0;i<pipecount;i++)
	{
		sound_pipe *pipe = &pipes[i];
		
		const char * cmd[6];
		cmd[0] = "/usr/local/bin/alsa-stream";
		cmd[1] = format_string;
		cmd[2] = rate_string;
		cmd[3] = pipe->input_device_name;
		cmd[4] = pipe->output_device_name;
		cmd[5] = NULL;
		err = start_child_process(cmd[0],cmd,&pipe->outStream,&pipe->inStream,&pipe->pid);
		if(err)
		{
			fprintf(stderr,"Can not fork subprocesses");
			return -1;
		}
        setNonBlocking(pipe->outStream);
        setNonBlocking(pipe->inStream);

	}
	
	int pollStructCount = 2*pipecount;
	size_t pSize = pollStructCount *sizeof(struct pollfd);
	struct pollfd *fds = malloc(pSize);
    int doQuit=0;
	while(doQuit==0)
	{
		fprintf(stderr,".");
		fflush(stderr);
		memset(fds,0x00,pSize);
		for(i=0;i<pipecount;i++)
		{
			sound_pipe *pipe = &pipes[i];
			fds[i*2+0].fd = pipe->controlStream;
			fds[i*2+0].events = POLLIN;
			fds[i*2+0].revents = 0;
			fds[i*2+1].fd = pipe->inStream;
			fds[i*2+1].events = POLLIN;
			fds[i*2+1].revents = 0;
		}
		int ret = poll(fds, pollStructCount, 1000); /* timeout of 1s */
		if (ret < 0)
    	{
        	if (errno != EINTR)
        	{
            	fprintf(stderr,"error in poll\n");
            	return -1;
        	}
        }
        for(i=0;i<pipecount;i++)
        {
            sound_pipe *pipe = &pipes[i];
            int control_events = fds[i*2+0].revents;
            int feedback_events = fds[i*2+1].revents;
            
            if(control_events & POLLIN)
            {
                char in;

                ssize_t n = read(pipe->controlStream,&in,1);
                if(n==1)
                {
                    if (in=='q')
                    {
                        doQuit=1;
                    }
					else if(in < 32)
					{
						/* unprintable controls like \n \r. we skip it */
					}
					else
					{
						fprintf(stderr,"%d:%s:%c\n",i,pipe->control_pipe_name,in);
						fflush(stderr);
                    	process_command(pipes,pipecount,i, in);
					}
                }
            }
            
            if(feedback_events & POLLIN)
            {
                char buf[256];
                ssize_t n = read(pipe->inStream,buf,sizeof(buf));
                while(n>0)
                {
                	fwrite(&buf[0],n,1,stdout);
					n = read(pipe->inStream,buf,sizeof(buf));
                    if(n>0)
                    {
                        fwrite(&buf,n,1,stdout);
                        fflush(stdout);
                    }
                }
            }
            if(feedback_events & POLLHUP)
            {
                fprintf(stderr,"Subprocess got killed\n");
				fflush(stdout);
            }
        }
    }

	for(i=0;i<pipecount;i++)
	{
		sound_pipe *pipe = &pipes[i];
		close(pipe->inStream);
		close(pipe->outStream);
		close(pipe->controlStream);
	}
	free(pipes);

	return 0;
}

void process_command(sound_pipe *pipes,int pipecount,int index, char in)
{
    int i;
    int max_priority = 0;
	
    if((in >='0') && (in <= '9'))
    {
        int level = pipes[index].currentPriorityLevel;

        switch(in)
        {
            case '1':
                level = 109;
                break;
            case '2':
                level = 108;
                break;
            case '3':
                level = 107;
                break;
            case '4':
                level = 106;
                break;
            case '5':
                level = 105;
                break;
            case '6':
                level = 104;
                break;
            case '7':
                level = 103;
                break;
            case '8':
                level = 102;
                break;
            case '9':
                level = 101;
                break;
            case '0':
                level = 0;
                break;
        }
        pipes[index].currentPriorityLevel = level;
    }
    
    /* find the highest priority in the system */
    for(i=0;i<pipecount;i++)
    {
        if(pipes[i].currentPriorityLevel  > max_priority)
        {
            max_priority = pipes[i].currentPriorityLevel;
        }
    }
    for(i=0;i<pipecount;i++)
    {
        if(pipes[i].currentPriorityLevel < max_priority)
        {
            mute(&pipes[i]);
        }
        else
        {
            unmute(&pipes[i]);
        }

    }
}

void mute(sound_pipe *pipe)
{
    if(pipe->muted==0)
    {
        write(pipe->outStream,"M",1);
        fprintf(stdout,"Muting %s->%s\n",pipe->input_device_name,pipe->output_device_name);
        fflush(stdout);
    }
    pipe->muted=1;
}

void unmute(sound_pipe *pipe)
{
    if(pipe->muted==1)
    {
        write(pipe->outStream,"U",1);
        fprintf(stdout,"Unmuting %s->%s\n",pipe->input_device_name,pipe->output_device_name);
        fflush(stdout);
    }
    pipe->muted=0;

}


void setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags  | O_NONBLOCK);
}
