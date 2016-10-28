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
#include "readline.h"
#include "dict.h"

#include <stddef.h>

#include <readline/readline.h>
#include <readline/history.h>

#define NAME "Palantir"

/**
 * Generator
 * @param text the text
 * @param state the state
 * @return text
 */
static char* generator(const char *text, int state) {
    static int i, len;

    if (state == 0) {
        i = 0; len = strlen(text);
    }

    const char *word;

    while ((word = dict[i++]) != NULL) {
        if (strncmp(word, text, len) == 0) {
            return strdup(word);
        }
    }

    return NULL;
}

/**
 * Completion
 * @param text the text
 * @param start the start position
 * @param end the end position
 * @return text
 */
static char** completion(const char *text, int start, int end) {
    rl_attempted_completion_over = 1;

    return rl_completion_matches(text, generator);
}

/**
 * Newline
 * @param count parameter
 * @param key code
 * @return success
 */
static int newline(int count, int key) {
    return rl_insert_text("\n");
}

/**
 * Readline init
 * @return success
 */
extern int readline_init() {
    rl_readline_name = NAME;
    rl_attempted_completion_function = completion;

    if (rl_bind_keyseq("\\C-n", newline) != 0) {
        return -1;
    }

    return 0;
}

/**
 * Readline prompt
 * @param prompt the prompt
 * @param line the line
 * @return success
 */
extern int readline_prompt(const char *prompt, char **line) {
    if ((*line = readline(prompt)) != NULL) {
        add_history(*line);
    } else {
        return -1;
    }

    return 0;
}
