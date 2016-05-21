/*
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
#include "palantir.h"
#include "lib/lua.h"
#include "lib/net.h"

#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "palantir.inc"

#define LUA_CODE ((const char*)palantir_luac)
#define LUA_SIZE ((size_t)palantir_luac_len)

static int is_daemon = 0;

/*
 * Palantir start
 */ 
static int palantir_start(const char *host, uint16_t port) {
    if (net_start() < 0) {
        return -1;
    }

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushstring(L, host);
    lua_setglobal(L, "HOST");

    lua_pushinteger(L, port);
    lua_setglobal(L, "PORT");

    lua_pushboolean(L, is_daemon);
    lua_setglobal(L, "DAEMON");

    lua_register(L, "connect", lua_connect);
    lua_register(L, "listen", lua_listen);
    lua_register(L, "accept", lua_accept);
    lua_register(L, "recv", lua_recv);
    lua_register(L, "send", lua_send);
    lua_register(L, "path", lua_path);
    lua_register(L, "sleep", lua_sleep);

    if (luaL_loadbuffer(L, LUA_CODE, LUA_SIZE, "lua") != LUA_OK) {
        fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
        return -1;
    }

    if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
        fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
        return -1;
    }

    lua_close(L);

    return 0;
}

/*
 * Palantir exit
 */ 
static void palantir_exit() {
    if (net_close() < 0) {
        perror("Palantir error");
    }
}

/*
 * Main
 */ 
int main(int argc, char *argv[]) {
    int opt, port = 0; char *t = NULL;

    while ((opt = getopt(argc, argv, "dhlv")) != -1) {
        switch (opt) {
            case 'd':
                is_daemon = 1;
                break;
            
            case 'h':
                printf("Usage: %s [-dhlv] IP PORT\n", argv[0]);
                exit(EXIT_SUCCESS);

            case 'l':
                printf("Licensed under the MIT License.\n");
                exit(EXIT_SUCCESS);

            case 'v':
                printf("palantir %s\n", VERSION);
                exit(EXIT_SUCCESS);

            default:
                exit(EXIT_FAILURE);
        }
    }

    if ((argc - optind) < 2) {
        fprintf(stderr, "Please give IP PORT\n");
        exit(EXIT_FAILURE);
    }

    if ((port = strtol(argv[optind + 1], &t, 0)) == 0) {
        fprintf(stderr, "Please give IP PORT\n");
        exit(EXIT_FAILURE);
    }

    atexit(palantir_exit);

    if (palantir_start(argv[optind], (uint16_t)port) < 0) {
        perror("Palantir error");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);        
}
