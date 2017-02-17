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
#include "args.h"

#include <string.h>

#define ARGS_PREFIX1 '-'
#define ARGS_PREFIX2 '/'

#define ARGS_STOP "--"
#define ARGS_VALUE ':'

static int i = 0;

/**
 * Args parse
 * @param arg the parsed argument
 * @param argc the argument count
 * @param argv the argument vector
 * @param format the option format
 * @return success
 */
extern int args_parse(arg_t *arg, int argc, char *argv[], const char *format) {
    if ((arg->index = ++i) >= argc) {
        return -1;
    }

    if (strcmp(argv[i], ARGS_STOP) == 0) {
        return -1;
    }

    char *f, *p = argv[i];

    switch (p[0]) {
        case ARGS_PREFIX1:
        case ARGS_PREFIX2:
            if ((f = strchr(format, (++p)[0])) == NULL) {
                return -1;
            }

            arg->param = f[0];

            if (f[1] == ARGS_VALUE) {
                if (++i == argc) {
                    return -1;
                }

                arg->value = argv[i];
            }
            return 0;

        default:
            return -1;
    }
}

/**
 * Args reset
 * @return success
 */
extern int args_reset() {
    return i = 0;
}
