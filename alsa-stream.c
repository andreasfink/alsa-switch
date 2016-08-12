/*
 * alsa-stream.c
 *
 * reading from audio capture device and writing to audio output device.
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

#include "alsa-switch-config.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#ifdef HAVE_ALSA_ASOUNDLIB_H

#include "alsa_util.h"
#include "system_util.h"


#define	DEFAULT_FORMAT	SND_PCM_FORMAT_S16_LE
#define	DEFAULT_RATE	48000
#define	BUFSIZE (DEFAULT_RATE * 8)

int main(int argc, const char *argv[])
{
	int err;
	unsigned int rate 	= DEFAULT_RATE;
	unsigned int format	= DEFAULT_FORMAT;
	snd_pcm_t *playback_handle = NULL;
	snd_pcm_t *capture_handle = NULL;
	char *buf;
	
	int buf_write_pos = 0;
	int buf_read_pos = 0;
	int buf_available = BUFSIZE;
	int buf_in_use = 0;
	int buf_size = BUFSIZE; /* this is in number of samples */
    int channels = 1; /* we are only working in mono channels */
    int sample_size;
    
	if(argc < 5)
	{
		fprintf(stderr,"\nUsage:\t%s audio-format sample-rate input-device output-device\n",argv[0]);
		return -1;
	}

	const char *input_device_name  = argv[3];
	const char *output_device_name = argv[4];
	const char *format_string = NULL;
	rate = atoi(argv[2]);
	format_string = argv[1];
	format = format_string_to_value(format_string);
	
	err = open_sound_device(&capture_handle, input_device_name, SND_PCM_STREAM_CAPTURE,rate,format,1024,0);
	if(err < 0)
	{
		return err;
	}
	err = open_sound_device(&playback_handle, output_device_name, SND_PCM_STREAM_PLAYBACK,rate,format,1024,0);
	if(err < 0)
	{
		return err;
	}
    
    sample_size = snd_pcm_format_size(format, bufsize) * channels;
    buf = calloc(1, sample_size);

	err = snd_pcm_start(capture_handle);
	if(err < 0)
	{
		fprintf(stderr, "cannot prepare audio interface for use(%s)\n",
			 snd_strerror(err));
		return err;
	}
    err = snd_pcm_prepare(playback_handle);
    if(err < 0)
    {
        fprintf(stderr, "cannot prepare audio interface for use(%s)\n",
                snd_strerror(err));
        return err;
    }
    
	setNonBlocking(STDIN_FILENO);

	snd_pcm_sframes_t in_avail;
	snd_pcm_sframes_t out_avail;


	int wait_read = 0;
	int work_done = 1;
	while (1)
	{
		if(!work_done)
		{
			/* if we had nothing to be done for one cycle, it might be whise to sleep a bit to not bring the system into a busyloop */
			/* but we should not sleep longer than 32 frames at 48kHz so we wont over or underrun the buffer */
			/* 32 frames at 48khz is 0.000667 seconds */
			usleep(666);
		}
	
		work_done = 0;
		if(wait_read)
		{
			err = snd_pcm_wait(playback_handle, 1000000);
			if(err < 0)
			{
				if(errno != EAGAIN)
				{
					fprintf(stderr, "poll failed(%s) errno %d\n", strerror(errno),errno);
					break;
				}
			}
			else
			{
				wait_read = 0;
				work_done++; /* we did read something */
			}
		}

		int cmd = getc(stdin);
		if(cmd != EOF)
		{
			printf("got command '%c'\n",cmd);
			fflush(stdout);
			work_done++;
		}

		in_avail = snd_pcm_avail_update(capture_handle);
		if (in_avail > 0)
		{
			int remaining_until_end = buf_size - buf_read_pos;
			
			if (in_avail > remaining_until_end)
			{
				in_avail = remaining_until_end;
			}
			if(in_avail > buf_available)
			{
				in_avail = buf_available;
			}
			snd_pcm_readi(capture_handle, &buf[buf_read_pos], in_avail);
			buf_read_pos += in_avail;
			buf_available -= in_avail;
			buf_read_pos = buf_read_pos % buf_size;
			buf_in_use += in_avail;
#ifdef DEBUG_BUFFER
			fprintf(stdout,"in  0x%08x  pos 0x%08x, avail 0x%08x\n",(unsigned int)in_avail,(unsigned int)buf_read_pos,(unsigned int)buf_available);
#endif
			work_done++;
		}

		out_avail = snd_pcm_avail_update(playback_handle);
		if (out_avail > 0)
		{
			if(out_avail > buf_in_use)
			{
				out_avail =buf_in_use;
			}	
			if (out_avail > buf_size)
			{
				out_avail = buf_size;
			}
			int remaining_until_end = buf_size - buf_write_pos;
			if(out_avail > remaining_until_end)
			{
				out_avail = remaining_until_end;
			}
			if(out_avail==0)
			{
				wait_read = 1;
				continue;
			}
			snd_pcm_writei(playback_handle, &buf[buf_write_pos], out_avail);
			buf_write_pos += out_avail;
			buf_available += out_avail;
			buf_write_pos %= buf_size;
			buf_in_use -= out_avail;
			work_done = 1;
#ifdef DEBUG_BUFFER
			fprintf(stdout,"out 0x%08x, pos 0x%08x avail 0x%08x\n",(unsigned int)out_avail,(unsigned int)buf_write_pos,(unsigned int)buf_available);
#endif		
			work_done++;
		}
	}
	snd_pcm_close(playback_handle);
	snd_pcm_close(capture_handle);
	return 0;
}
#else
int main(int argc, const char *argv[])
{
	fprintf(stdout,"dummy aslsa-stream\n");
	sleep(3);
	while(1)
	{
		printf("=");
		fflush(stdout);
		sleep(3);
	}
}
#endif

