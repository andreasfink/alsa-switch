/*
 * system_util.c
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
 
#include "system_util.h"
#include <string.h>
#include <stdlib.h>

#define RXPIPE 0
#define TXPIPE 1

/* 
** start_child_process
** 
** this starts a child process which is attached to its parent process through pipes.
** the parent can write or read the file descriptors returned to write/read to the
** standard input and output of the child process
*/

int start_child_process(char *const *cmd, int *toChildFd, int *fromChildFd, pid_t *childPid)
{
    int pipe_down[2];
    int pipe_up[2];
	pid_t pid;
	if(pipe(pipe_down)< 0)
	{
		return -1;
	}
    if(pipe(pipe_up)< 0)
    {
        return -1;
    }
	pid = fork();
	if(pid==-1)
	{
		return -1;
	}
	if(pid==0)
	{
		/* this is the child process */
        /* we map the pipes to its standardin / standardout file descriptors */
		dup2(pipe_down[RXPIPE], STDIN_FILENO);
        close(pipe_down[TXPIPE]);

		dup2(pipe_up[TXPIPE], STDOUT_FILENO);
        close(pipe_up[RXPIPE]);

		if (execvp(cmd[0], cmd) == -1)
		{
			exit(-1);
		}
		exit(0);
	}
	else
	{
		*fromChildFd = pipe_up[RXPIPE];
		*toChildFd = pipe_down[TXPIPE];
		*childPid = pid;
	}
	return 0;
}
