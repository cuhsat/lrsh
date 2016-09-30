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
#include "os.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#ifdef READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

/**
 * OS readline
 * @param rl the readline address
 * @return success
 */ 
extern int os_readline(readline_t *rl) {
    char *buffer = NULL;

    #ifdef READLINE

    if ((buffer = readline(rl->prompt)) != NULL) {
        add_history(buffer);
    } else {
        return -1;        
    }

    #else // READLINE

    size_t size = 0;

    printf("%s ", rl->prompt);

    if (getline(&buffer, &size, stdin) < 0) {
        return -1;
    }

    #endif // READLINE

    strncpy(rl->line, buffer, sizeof(rl->line));
    free(buffer);

    return 0;
}

/**
 * OS sleep
 * @param time the time object
 * @return success
 */ 
extern int os_sleep(time_t time) {
    struct timespec ts;

    ts.tv_sec  = (time / 1000);
    ts.tv_nsec = (time % 1000) * 1000000L;

    while (nanosleep(&ts, &ts) < 0) {
        continue;
    }

    return 0;
}

/**
 * OS env
 * @param env the env address
 * @return success
 */ 
extern int os_env(env_t *env) {
    if (strnlen(env->path, sizeof(env->path)) && chdir(env->path) < 0) {
        return -1;
    }    

    if (getlogin_r(env->user, sizeof(env->user)) < 0) {
        return -1;
    }

    if (gethostname(env->host, sizeof(env->host)) < 0) {
        return -1;
    }

    if (getcwd(env->path, sizeof(env->path)) == NULL) {
        return -1;
    }

    return 0;
}
