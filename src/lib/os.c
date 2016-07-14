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

#ifdef READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

/**
 * OS handler
 * @param error the error message
 * @return success
 */ 
extern int os_handler(const char *error) {
    fprintf(stderr, "Palantir error: %s\n", error);

    return 0;
}

/**
 * OS prompt
 * @param prompt the prompt address
 * @return success
 */ 
extern int os_prompt(prompt_t *prompt) {
    char *buffer = NULL;

    #ifdef READLINE

    if ((buffer = readline(prompt->prompt)) != NULL) {
        add_history(buffer);
    } else {
        return -1;        
    }

    #else

    size_t size = 0;

    printf("%s ", prompt->prompt);

    if (getline(&buffer, &size, stdin) < 0) {
        return -1;
    }

    #endif

    strncpy(prompt->line, buffer, sizeof(prompt->line));
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
 * OS info
 * @param info the info address
 * @return success
 */ 
extern int os_info(info_t *info) {
    if (strnlen(info->path, sizeof(info->path)) && chdir(info->path) < 0) {
        return -1;
    }    

    if (getlogin_r(info->user, sizeof(info->user)) < 0) {
        return -1;
    }

    if (gethostname(info->host, sizeof(info->host)) < 0) {
        return -1;
    }

    if (getcwd(info->path, sizeof(info->path)) == NULL) {
        return -1;
    }

    return 0;
}
