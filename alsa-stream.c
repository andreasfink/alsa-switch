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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "util.h"

#define BUFSIZE (1024 * 4)
#define	DEFAULT_FORMAT	SND_PCM_FORMAT_S16_LE
#define	DEFAULT_RATE	48000


snd_pcm_t *playback_handle, *capture_handle;
int buf[BUFSIZE * 2];


int main(int argc, const char *argv[])
{
	int err;
	unsigned int rate 	= DEFAULT_RATE;
	unsigned int format	= DEFAULT_FORMAT;

	if(argc < 3)
	{
		fprintf(stderr,"Usage:\n\t%s input-device output-device {sample-rate (default 8000)} {format ( defaults to S16_LE)}\n",argv[0]);
		return -1;
	}

	const char *input_device_name  = argv[1];
	const char *output_device_name = argv[2];
	const char *format_string = NULL;
	if(argc > 3)
	{
		rate = atoi(argv[3]);
	}
	if(argc > 4)
	{
		format_string = argv[4];
		format = format_string_to_value(format_string);
	}
	err = open_sound_device(&capture_handle, input_device_name, SND_PCM_STREAM_CAPTURE,rate,format);
	if(err < 0)
	{
		return err;
	}
	err = open_sound_device(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK,rate,format);
	if(err < 0)
	{
		return err;
	}
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

	memset(buf, 0, sizeof(buf));

	while (1)
	{
		int avail;
		err = snd_pcm_wait(playback_handle, 1000);
		if(err < 0)
		{
			fprintf(stderr, "poll failed(%s)\n", strerror(errno));
			break;
		}

		avail = snd_pcm_avail_update(capture_handle);
		if (avail > 0)
		{
			if (avail > BUFSIZE)
			{
				avail = BUFSIZE;
			}
			snd_pcm_readi(capture_handle, buf, avail);
		}

		avail = snd_pcm_avail_update(playback_handle);
		if (avail > 0)
		{
			if (avail > BUFSIZE)
			{
				avail = BUFSIZE;
			}
			snd_pcm_writei(playback_handle, buf, avail);
		}
	}
	snd_pcm_close(playback_handle);
	snd_pcm_close(capture_handle);
	return 0;
}

