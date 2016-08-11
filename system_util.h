/*
 * system_util.c
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

#include <unistd.h> /* For pid_t */
#include <stdlib.h>

int start_child_process(const char *binary, char *const *cmd, int *toChildFdr, int *fromChildFd, pid_t *childPid);
