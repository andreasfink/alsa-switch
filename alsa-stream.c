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


int main(int argc, const char *argv[])
{
	int err;
	snd_pcm_t *playback_handle = NULL;
	snd_pcm_t *capture_handle = NULL;
	int buf_byte_size = 0;
	char *buf = NULL;
	unsigned int rate = 48000;
	int format = SND_PCM_FORMAT_S16_LE;
	snd_pcm_uframes_t buf_sample_size 			= 8192; /* this is in number of samples we want to store in our buffer */
	snd_pcm_uframes_t buf_sample_size_capture = buf_sample_size;
	snd_pcm_uframes_t buf_sample_size_playback = buf_sample_size;
	int channels 								= 1; /* we are only working in mono channels */

	if(argc < 3)
	{
		fprintf(stderr,"\nUsage:\t%s input-device output-device\n",argv[0]);
		return -1;
	}
	const char *input_device_name  = argv[1];
	const char *output_device_name = argv[2];
	err = open_sound_device(&capture_handle,
								 input_device_name,
								 SND_PCM_STREAM_CAPTURE,
								 rate,
								 format,
								 &buf_sample_size_capture,
								 NULL,
								 1);
	if(err < 0)
	{
		return err;
	}
	err = open_sound_device(&playback_handle, 
								output_device_name,
								SND_PCM_STREAM_PLAYBACK,
								rate,
								format,
								&buf_sample_size_playback,
								NULL,
								1);
	if(err < 0)
	{
		return err;
	}

	if(buf_sample_size_playback > buf_sample_size_capture)
	{
		buf_sample_size = buf_sample_size_capture;
	}
	else
	{
		buf_sample_size = buf_sample_size_playback;
	}
	
	printf("Samples in capture buffer: %d\n",(int)buf_sample_size_capture);
	printf("Samples in playback buffer: %d\n",(int)buf_sample_size_playback);
	printf("Samples in buffer: %d\n",(int)buf_sample_size);
	
    buf_byte_size = snd_pcm_format_size(format, buf_sample_size) * channels;
    buf = calloc(1, buf_byte_size);

    err = snd_pcm_prepare(playback_handle);
    if(err < 0)
    {
        fprintf(stderr, "cannot prepare audio interface for use(%s)\n",
                snd_strerror(err));
        return err;
    }
    
	err = snd_pcm_start(capture_handle);
	if(err < 0)
	{
		fprintf(stderr, "cannot prepare audio interface for use(%s)\n",
			 snd_strerror(err));
		return err;
	}
	setNonBlocking(STDIN_FILENO);

	snd_pcm_sframes_t in_avail;
	snd_pcm_sframes_t out_avail;

	int mute = 0;
	if(1)
	{
		int wait_timeout_ms = 100;
		err = snd_pcm_wait(capture_handle, wait_timeout_ms);
		if(err == -EPIPE)
		{
			/* xrun */
		}
		else if(err == -ESTRPIPE)
		{
		}
		else if(err < 0)
		{
			fprintf(stderr, "poll failed(%s) errno %d\n", strerror(errno),errno);
			return -1;
		}
	}
	while (1)
	{
	

		/* lets read samples up to the buffer size */
		in_avail = snd_pcm_avail_update(capture_handle);
		if (in_avail > 0)
		{
			if(in_avail > buf_sample_size)
			{
				in_avail = buf_sample_size;
			}	
			snd_pcm_readi(capture_handle, buf, in_avail);
			out_avail = snd_pcm_avail_update(playback_handle);
			while (out_avail < in_avail)
			{
				usleep(100);
				out_avail = snd_pcm_avail_update(playback_handle);
			}

			if(mute==1)
			{
				/* muting means we output zeros */
				memset(&buf,0x00,buf_byte_size);
			}
			snd_pcm_writei(playback_handle, buf, in_avail);
		}
		else
		{
			usleep(100);
		}
		
		int cmd = getc(stdin);
		if(cmd != EOF)
		{
			switch(cmd)
			{
				case 'm':
				case 'M':
					mute=1;
					printf("<muted>\n");
					break;
				case 'u':
				case 'U':
					mute=0;
					printf("<umuted>\n");
					break;
			}
			fflush(stdout);	
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

