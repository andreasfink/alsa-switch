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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "util.h"

#define BUFSIZE (1024 * 4)

typedef struct sound_pipe
{
	const char *input_device_name;
	const char *output_device_name;
	const char *control_pipe_name;
	snd_pcm_t *input_handle;
	snd_pcm_t *output_handle;
	int control_pipe_fd;
	int buf[BUFSIZE * 2];
} sound_pipe;

int mkfifo(const char *pathname, mode_t mode);

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
	int rate = atoi(argv[2]);
	int format = format_string_to_value(format_string);

	int pipecount = (argc - 3)/3;
	if(((argc -3) % 3) != 0)
	{
		fprintf(stderr,"Usage:\n\t%s input-device1 output-device1 control-pipe1 input-device2 output-device2 control-pipe2 ...\n",argv[0]);
		return -1;
	}
	if((pipecount > 256) || (pipecount < 1))
	{
		fprintf(stderr,"Sorry but zero or less or more than 256 sound pipes is unrealistic\n",argv[0]);
		return -1;
	}

	sound_pipe *pipes = calloc(sizeof(sound_pipe),pipecount);	
	for(i=0;i<pipecount;i++)
	{
		sound_pipe *pipe = &pipes[i];
		
		pipe->input_device_name = argv[3+(i*3)+0];
		pipe->output_device_name = argv[3+(i*3)+1];
		pipe->control_pipe_name = argv[3+(i*3)+2];
		memset(pipe->buf, 0, sizeof(pipe->buf));
	
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
		pipe->control_pipe_fd = open(pipe->control_pipe_name,O_RDONLY);
		if(pipe->control_pipe_fd < 0)
		{
			fprintf(stderr,"I can not open %s for reading\n",pipe->control_pipe_name);
			return errno;
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
	}
	while (1)
	{
		for(i=0;i<pipecount;i++)
		{
			int avail;

			sound_pipe *pipe = &pipes[i];

			err = snd_pcm_wait(pipe->output_handle, 1000);
			if(err < 0)
			{
				fprintf(stderr, "poll failed(%s)\n", strerror(errno));
				break;
			}
		
			avail = snd_pcm_avail_update(pipe->input_handle);
			if (avail > 0)
			{
				if (avail > BUFSIZE)
				{
					avail = BUFSIZE;
				}
				snd_pcm_readi(pipe->input_handle, pipe->buf, avail);
			}

			avail = snd_pcm_avail_update(pipe->output_handle);
			if (avail > 0)
			{
				if (avail > BUFSIZE)
				{
					avail = BUFSIZE;
				}
				snd_pcm_writei(pipe->output_handle, pipe->buf, avail);
			}
		}
		
	}
	
	for(i=0;i<pipecount;i++)
	{
		sound_pipe *pipe = &pipes[i];
		snd_pcm_close(pipe->output_handle);
		snd_pcm_close(pipe->input_handle);
		close(pipe->control_pipe_fd);
	}
	free(pipe);

	return 0;
}

