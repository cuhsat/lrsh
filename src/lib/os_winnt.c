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

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

/**
 * OS init
 * @param mode the mode
 * @return success
 */
extern int os_init(int mode) {
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
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
    if (ShowWindow(GetConsoleWindow(), debug) == 0) {
        return -1;
    }

    if (SetCurrentDirectoryA("C:\\") == 0) {
        return -1;
    }

    return 0;
}

/**
 * OS path
 * @param path the path address
 * @return success
 */
extern int os_path(path_t *path) {
    if (strlen(path->path) && SetCurrentDirectoryA(path->path) == 0) {
        return -1;
    }

    DWORD user_size = sizeof(path->user);

    if (GetUserNameA(path->user, &user_size) == 0) {
        return -1;
    }

    DWORD host_size = sizeof(path->host);

    if (GetComputerNameA(path->host, &host_size) == 0) {
        return -1;
    }

    if (GetCurrentDirectoryA(sizeof(path->path), path->path) == 0) {
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
    printf("%s", prompt->prompt);

    if (fgets(prompt->line, sizeof(prompt->line), stdin) == NULL) {
        return -1;
    }

    prompt->line[strcspn(prompt->line, "\n")] = 0;

    return 0;
}

/**
 * OS sleep
 * @param time the time object
 * @return success
 */
extern int os_sleep(time_t time) {
    if (SleepEx((DWORD)time, FALSE) != 0) {
        return -1;
    }

    return 0;
}

/**
 * OS exit
 * @return success
 */
extern int os_exit() {
    if (WSACleanup() != 0) {
        return -1;
    }

    return 0;
}
