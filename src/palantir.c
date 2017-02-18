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
#include "palantir.h"
#include "lib/args.h"
#include "lib/net.h"
#include "lib/os.h"
#include "lua/lua.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../build/palantir.inc"

#define LUA_CODE ((const char *)palantir_luac)
#define LUA_SIZE ((size_t)palantir_luac_len)

typedef enum {
    MODE_CLIENT = 0,
    MODE_SERVER = 1
} palantir_mode;

static palantir_mode mode = MODE_SERVER;
static const char *token = "";
static const char *stack = "";

/**
 * Palantir start
 * @param host the host address (name or ip)
 * @param port the port number
 * @return success
 */
static int palantir_start(const char *host, uint16_t port) {
    lua_State *L = luaL_newstate();
    lua_atpanic(L, lua_panic);

    if (os_start(mode) < 0) {
        return -1;
    }

    if (net_auth(token) < 0) {
        return -1;
    }

    luaL_openlibs(L);

    lua_createtable(L, 0, 10);
    lua_pushboolean(L, mode);
    lua_setfield(L, -2, "MODE");
    lua_pushstring(L, host);
    lua_setfield(L, -2, "HOST");
    lua_pushinteger(L, port);
    lua_setfield(L, -2, "PORT");
    lua_pushstring(L, stack);
    lua_setfield(L, -2, "STACK");
    lua_pushstring(L, token);
    lua_setfield(L, -2, "TOKEN");
    lua_pushstring(L, BUILD);
    lua_setfield(L, -2, "BUILD");
    lua_pushboolean(L, DEBUG);
    lua_setfield(L, -2, "DEBUG");
    lua_pushstring(L, VERSION);
    lua_setfield(L, -2, "VERSION");

    lua_createtable(L, 0, 5);
    lua_pushcfunction(L, lua_connect);
    lua_setfield(L, -2, "connect");
    lua_pushcfunction(L, lua_listen);
    lua_setfield(L, -2, "listen");
    lua_pushcfunction(L, lua_accept);
    lua_setfield(L, -2, "accept");
    lua_pushcfunction(L, lua_recv);
    lua_setfield(L, -2, "recv");
    lua_pushcfunction(L, lua_send);
    lua_setfield(L, -2, "send");
    lua_setfield(L, -2, "net");

    lua_createtable(L, 0, 3);
    lua_pushcfunction(L, lua_prompt);
    lua_setfield(L, -2, "prompt");
    lua_pushcfunction(L, lua_sleep);
    lua_setfield(L, -2, "sleep");
    lua_pushcfunction(L, lua_env);
    lua_setfield(L, -2, "env");
    lua_setfield(L, -2, "os");

    lua_setglobal(L, "_P");

    if (luaL_loadbuffer(L, LUA_CODE, LUA_SIZE, "lua") != 0) {
        fprintf(stderr, "Palantir error: %s\n", lua_tostring(L, -1));
        return -1;
    }

    if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0) {
        fprintf(stderr, "Palantir error: %s\n", lua_tostring(L, -1));
        return -1;
    }

    lua_close(L);
    return 0;
}

/**
 * Palantir exit
 */
static void palantir_exit() {
    if (net_exit() < 0) {
        perror("Palantir error");
    }
}

/**
 * Main
 * @param argc the command line arguments count
 * @param argv the command line arguments array
 * @return exit code
 */
int main(int argc, char *argv[]) {
    arg_t arg; int port = 0; char *t = NULL;

    while (args_parse(&arg, argc, argv, "dhlva:c:f:") == 0) {
        switch (arg.param) {
            case 'c':
            case 'f':
                stack = arg.value;
                break;

            case 'a':
                token = arg.value;
                break;

            case 'd':
                mode = MODE_CLIENT;
                break;

            case 'h':
                printf("Usage: palantir [-dhlv] [-a TOKEN] [-c COMMAND] [-f FILE] HOST PORT\n");
                exit(EXIT_SUCCESS);

            case 'l':
                printf("Licensed under the MIT License.\n");
                exit(EXIT_SUCCESS);

            case 'v':
                printf("Palantir %s (%s)\n", VERSION, LUA_VERSION);
                exit(EXIT_SUCCESS);

            default:
                exit(EXIT_FAILURE);
        }
    }

    if ((argc - arg.index) < 2) {
        fprintf(stderr, "Please give HOST and PORT\n");
        exit(EXIT_FAILURE);
    }

    if ((port = strtol(argv[arg.index + 1], &t, 0)) == 0) {
        fprintf(stderr, "Please give HOST and PORT\n");
        exit(EXIT_FAILURE);
    }

    if (mode == MODE_CLIENT && os_daemon(DEBUG) < 0) {
        perror("Palantir error");
        exit(EXIT_FAILURE);
    }

    atexit(palantir_exit);

    if (palantir_start(argv[arg.index], (uint16_t)port) < 0) {
        perror("Palantir error");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
