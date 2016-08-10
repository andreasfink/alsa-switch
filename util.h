/*
 * util.c
 *
 * generic utility functions
  *
 * Copyright (c) 2016 Andreas Fink
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */
 
#include <alsa/asoundlib.h>
 
int format_string_to_value(const char *s);
int open_sound_device(snd_pcm_t **handle, const char *name, int dir, int rate, int format, size_t bufsize);