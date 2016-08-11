/*
 * util.c
 *
 * generic utility functions
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

#ifdef	HAVE_ALSA_ASOUNDLIB_H

#include "alsa_util.h"
#include <string.h>

int format_string_to_value(const char *s)
{
	if(s == NULL)
	{
		return SND_PCM_FORMAT_FLOAT;
	}
	if( *s == '\0')
	{
		return SND_PCM_FORMAT_FLOAT;
	}
    if(strcmp("S8",s)==0)
	{
		return SND_PCM_FORMAT_S8;
	}
    if(strcmp("U8",s)==0)
	{
		return SND_PCM_FORMAT_U8;
	}
    if(strcmp("S16_LE",s)==0)
	{
		return SND_PCM_FORMAT_S16_LE;
	}
    if(strcmp("S16_BE",s)==0)
	{
		return SND_PCM_FORMAT_S16_BE;
	}
    if(strcmp("U16_LE",s)==0)
	{
		return SND_PCM_FORMAT_U16_LE;
	}
    if(strcmp("U16_BE",s)==0)
	{
		return SND_PCM_FORMAT_U16_BE;
	}
    if(strcmp("S24_LE",s)==0)
	{
		return SND_PCM_FORMAT_S24_LE;
	}
    if(strcmp("S24_BE",s)==0)
	{
		return SND_PCM_FORMAT_S24_BE;
	}
    if(strcmp("U24_LE",s)==0)
	{
		return SND_PCM_FORMAT_U24_LE;
	}
    if(strcmp("U24_BE",s)==0)
	{
		return SND_PCM_FORMAT_U24_BE;
	}
    if(strcmp("S32_LE",s)==0)
	{
		return SND_PCM_FORMAT_S32_LE;
	}
    if(strcmp("S32_BE",s)==0)
	{
		return SND_PCM_FORMAT_S32_BE;
	}
    if(strcmp("U32_LE",s)==0)
	{
		return SND_PCM_FORMAT_U32_LE;
	}
    if(strcmp("U32_BE",s)==0)
	{
		return SND_PCM_FORMAT_U32_BE;
	}
    if(strcmp("FLOAT_LE",s)==0)
	{
		return SND_PCM_FORMAT_FLOAT_LE;
	}
    if(strcmp("FLOAT_BE",s)==0)
	{
		return SND_PCM_FORMAT_FLOAT_BE;
	}
    if(strcmp("FLOAT64_LE",s)==0)
	{
		return SND_PCM_FORMAT_FLOAT64_LE;
	}
    if(strcmp("FLOAT64_BE",s)==0)
	{
		return SND_PCM_FORMAT_FLOAT64_BE;
	}
    if(strcmp("IEC958_SUBFRAME_LE",s)==0)
	{
		return SND_PCM_FORMAT_IEC958_SUBFRAME_LE;
	}
    if(strcmp("IEC958_SUBFRAME_BE",s)==0)
	{
		return SND_PCM_FORMAT_IEC958_SUBFRAME_BE;
	}
    if(strcmp("MU_LAW",s)==0)
	{
		return SND_PCM_FORMAT_MU_LAW;
	}
    if(strcmp("A_LAW",s)==0)
	{
		return SND_PCM_FORMAT_A_LAW;
	}
    if(strcmp("IMA_ADPCM",s)==0)
	{
		return SND_PCM_FORMAT_IMA_ADPCM;
	}
    if(strcmp("MPEG",s)==0)
	{
		return SND_PCM_FORMAT_MPEG;
	}
    if(strcmp("GSM",s)==0)
	{
		return SND_PCM_FORMAT_GSM;
	}
    if(strcmp("SPECIAL",s)==0)
	{
		return SND_PCM_FORMAT_SPECIAL;
	}
    if(strcmp("S24_3LE",s)==0)
	{
		return SND_PCM_FORMAT_S24_3LE;
	}
    if(strcmp("S24_3BE",s)==0)
	{
		return SND_PCM_FORMAT_S24_3BE;
	}
    if(strcmp("U24_3LE",s)==0)
	{
		return SND_PCM_FORMAT_U24_3LE;
	}
    if(strcmp("U24_3BE",s)==0)
	{
		return SND_PCM_FORMAT_U24_3BE;
	}
    if(strcmp("S20_3LE",s)==0)
	{
		return SND_PCM_FORMAT_S20_3LE;
	}
    if(strcmp("S20_3BE",s)==0)
	{
		return SND_PCM_FORMAT_S20_3BE;
	}
    if(strcmp("U20_3LE",s)==0)
	{
		return SND_PCM_FORMAT_U20_3LE;
	}
    if(strcmp("U20_3BE",s)==0)
	{
		return SND_PCM_FORMAT_U20_3BE;
	}
    if(strcmp("S18_3LE",s)==0)
	{
		return SND_PCM_FORMAT_S18_3LE;
	}
    if(strcmp("S18_3BE",s)==0)
	{
		return SND_PCM_FORMAT_S18_3BE;
	}
    if(strcmp("U18_3LE",s)==0)
	{
		return SND_PCM_FORMAT_U18_3LE;
	}
    if(strcmp("U18_3BE",s)==0)
	{
		return SND_PCM_FORMAT_U18_3BE;
	}
    if(strcmp("S16",s)==0)
	{
		return SND_PCM_FORMAT_S16;
	}
    if(strcmp("U16",s)==0)
	{
		return SND_PCM_FORMAT_U16;
	}
    if(strcmp("S24",s)==0)
	{
		return SND_PCM_FORMAT_S24;
	}
    if(strcmp("U24",s)==0)
	{
		return SND_PCM_FORMAT_U24;
	}
    if(strcmp("S32",s)==0)
	{
		return SND_PCM_FORMAT_S32;
	}
    if(strcmp("U32",s)==0)
	{
		return SND_PCM_FORMAT_U32;
	}
    if(strcmp("FLOAT",s)==0)
	{
		return SND_PCM_FORMAT_FLOAT;
	}
    if(strcmp("FLOAT64",s)==0)
	{
		return SND_PCM_FORMAT_FLOAT64;
	}
    if(strcmp("IEC958_SUBFRAME",s)==0)
	{
		return SND_PCM_FORMAT_IEC958_SUBFRAME;
	}
	return SND_PCM_FORMAT_FLOAT;
} 

int open_sound_device(snd_pcm_t **handle, const char *name, int dir, unsigned int rate, int format, size_t bufsize)
{
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sw_params_t *sw_params;
	const char *dirname = (dir == SND_PCM_STREAM_PLAYBACK) ? "PLAYBACK" : "CAPTURE";
	int err;

	err = snd_pcm_open(handle, name, dir, 0);
	if (err < 0) 
	{
		fprintf(stderr, "%s (%s): cannot open audio device (%s)\n", 
			name, dirname, snd_strerror(err));
		return err;
	}
	
	err = snd_pcm_hw_params_malloc(&hw_params);
	if(err < 0) 
	{
		fprintf(stderr, "%s (%s): cannot allocate hardware parameter structure(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}
			 
	err = snd_pcm_hw_params_any(*handle, hw_params);
	if(err < 0)
	{
		fprintf(stderr, "%s (%s): cannot initialize hardware parameter structure(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}

	err = snd_pcm_hw_params_set_access(*handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if(err < 0)
	{
		fprintf(stderr, "%s (%s): cannot set access type(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}

	err = snd_pcm_hw_params_set_format(*handle, hw_params, format);
	if(err < 0)
	{
		fprintf(stderr, "%s (%s): cannot set sample format(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}

	err = snd_pcm_hw_params_set_rate_near(*handle, hw_params, &rate, NULL);
	if(err < 0)
	{
		fprintf(stderr, "%s (%s): cannot set sample rate(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}

	err = snd_pcm_hw_params_set_channels(*handle, hw_params, 1);
	if(err < 0)
	{
		fprintf(stderr, "%s (%s): cannot set channel count(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}

	err = snd_pcm_hw_params(*handle, hw_params);
	if(err< 0) 
	{
		fprintf(stderr, "%s (%s): cannot set parameters(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}

	snd_pcm_hw_params_free(hw_params);

	err = snd_pcm_sw_params_malloc(&sw_params);
	if(err < 0) 
	{
		fprintf(stderr, "%s (%s): cannot allocate software parameters structure(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}
	err = snd_pcm_sw_params_current(*handle, sw_params);
	if(err < 0) 
	{
		fprintf(stderr, "%s (%s): cannot initialize software parameters structure(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}
	err = snd_pcm_sw_params_set_avail_min(*handle, sw_params, 64);
	if(err < 0) 
	{
		fprintf(stderr, "%s (%s): cannot set minimum available count(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}
	err = snd_pcm_sw_params_set_start_threshold(*handle, sw_params, 0U);
	if(err < 0)
	{
		fprintf(stderr, "%s (%s): cannot set start mode(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}
	err = snd_pcm_sw_params(*handle, sw_params);
	if(err < 0)
	{
		fprintf(stderr, "%s (%s): cannot set software parameters(%s)\n",
			name, dirname, snd_strerror(err));
		return err;
	}
	return 0;
}

#endif
