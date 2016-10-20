/**
 * Copyright (c) 2016 Christian Uhsat <christian@uhsat.de>
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef LIB_OS_H
#define LIB_OS_H

#include <limits.h>
#include <time.h>

#if (defined(DEBUG) && (DEBUG == 1))
#undef DAEMON
#endif

#ifdef __APPLE__
#undef DAEMON
#endif

#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX 255
#endif

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255
#endif

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

#define MAX_LINE 1024
#define MAX_USER (LOGIN_NAME_MAX + 1)
#define MAX_HOST (HOST_NAME_MAX + 1)
#define MAX_PATH (PATH_MAX + 1)

typedef struct {
    const char* prompt;
    char line[MAX_LINE];
} readline_t;

typedef struct {
    char user[MAX_USER];
    char host[MAX_HOST];
    char path[MAX_PATH];
} env_t;

extern int os_readline(readline_t *rl);
extern int os_sleep(time_t time);
extern int os_env(env_t *env);
extern int os_daemon();

#endif // LIB_OS_H
