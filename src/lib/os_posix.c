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
 * OS start
 * @param mode the mode
 * @return success
 */
extern int os_start(int mode) {
#ifdef HAVE_READLINE

    if (mode == 1 && readline_init() < 0) {
        return -1;
    }

#endif // HAVE_READLINE

    return 0;
}

/**
 * OS prompt
 * @param prompt the prompt address
 * @return success
 */
extern int os_prompt(prompt_t *prompt) {
#ifdef HAVE_READLINE

    char *buffer = NULL;

    if (readline_prompt(prompt->prompt, &buffer) < 0) {
        return -1;
    }

    strncpy(prompt->line, buffer, sizeof(prompt->line));

    free(buffer);

#else // HAVE_READLINE

    printf("%s ", prompt->prompt);

    if (fgets(prompt->line, sizeof(prompt->line), stdin) == NULL) {
        return -1;
    }

#endif // HAVE_READLINE

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
    struct passwd *pw;

    if (strlen(env->path) && chdir(env->path) < 0) {
        return -1;
    }

    if ((pw = getpwuid(getuid())) == NULL) {
        return -1;
    }

    strncpy(env->user, pw->pw_name, strlen(pw->pw_name));

    if (gethostname(env->host, sizeof(env->host)) < 0) {
        return -1;
    }

    if (getcwd(env->path, sizeof(env->path)) == NULL) {
        return -1;
    }

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
