/**
 * Copyright (c) 2016-2017 Christian Uhsat <christian@uhsat.de>
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

#ifdef HAVE_READLINE
#include "readline.h"
#endif

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

/**
 * OS init
 * @param mode the mode
 * @return success
 */
extern int os_init(int mode) {
#ifdef HAVE_READLINE

    if (mode == 0 && readline_init() < 0) {
        return -1;
    }

#endif // HAVE_READLINE

    return 0;
}

/**
 * OS daemon
 * @param debug the debug flag
 * @return success
 */
extern int os_daemon(int debug) {
#ifdef HAVE_DAEMON

    return daemon(0, debug);

#else // HAVE_DAEMON

    return chdir("/");

#endif // HAVE_DAEMON
}

/**
 * OS path
 * @param path the path address
 * @return success
 */
extern int os_path(path_t *path) {
    struct passwd *pw;

    if (strlen(path->path) && chdir(path->path) < 0) {
        return -1;
    }

    if ((pw = getpwuid(getuid())) == NULL) {
        return -1;
    }

    strncpy(path->user, pw->pw_name, strlen(pw->pw_name));

    if (gethostname(path->host, sizeof(path->host)) < 0) {
        return -1;
    }

    if (getcwd(path->path, sizeof(path->path)) == NULL) {
        return -1;
    }

    return 0;
}

/**
 * OS prompt
 * @param prompt the prompt address
 * @return success
 */
extern int os_prompt(prompt_t *prompt) {
    char *buffer = NULL;

#ifdef HAVE_READLINE

    if (readline_prompt(prompt->prompt, &buffer) < 0) {
        return -1;
    }

#else // HAVE_READLINE

    size_t size = 0;

    printf("%s", prompt->prompt);

    if (getline(&buffer, &size, stdin) < 0) {
        return -1;
    }

#endif // HAVE_READLINE

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
 * OS exit
 * @return success
 */
extern int os_exit() {
    return 0;
}
