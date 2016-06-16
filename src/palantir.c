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

#define LUA_CODE ((const char*)src_palantir_luac)
#define LUA_SIZE ((size_t)src_palantir_luac_len)
#define TIMEOUT 5000

typedef enum {
    MODE_PASSIVE,
    MODE_ACTIVE
} palantir_mode;

static palantir_mode mode = MODE_ACTIVE;

/*
 * Palantir start
 */ 
static int palantir_start(const char *host, uint16_t port) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_createtable(L, 0, 13);

    lua_pushcfunction(L, lua_connect);
    lua_setfield(L, -2, "connect");

    lua_pushcfunction(L, lua_listen);
    lua_setfield(L, -2, "listen");

    lua_pushcfunction(L, lua_accept);
    lua_setfield(L, -2, "accept");

    lua_pushcfunction(L, lua_recv);
    lua_setfield(L, -2, "raw_recv");
    
    lua_pushcfunction(L, lua_send);
    lua_setfield(L, -2, "raw_send");
    
    lua_pushcfunction(L, lua_sleep);
    lua_setfield(L, -2, "sleep");

    lua_pushcfunction(L, lua_system);
    lua_setfield(L, -2, "system");
    
    lua_pushboolean(L, mode);
    lua_setfield(L, -2, "mode");

    lua_pushstring(L, host);
    lua_setfield(L, -2, "host");

    lua_pushinteger(L, port);
    lua_setfield(L, -2, "port");

    lua_pushboolean(L, DEBUG);
    lua_setfield(L, -2, "debug");

    lua_pushinteger(L, TIMEOUT);
    lua_setfield(L, -2, "timeout");

    lua_pushstring(L, VERSION);
    lua_setfield(L, -2, "version");

    lua_setglobal(L, "palantir");

    if (luaL_loadbuffer(L, LUA_CODE, LUA_SIZE, "lua") != LUA_OK) {
        fprintf(stderr, "Palantir error: %s\n", lua_tostring(L, -1));
        return -1;
    }

    if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
        fprintf(stderr, "Palantir error: %s\n", lua_tostring(L, -1));
        return -1;
    }

    lua_close(L);
    return 0;
}

/*
 * Palantir exit
 */ 
static void palantir_exit() {
    if (net_exit() < 0) {
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
                mode = MODE_PASSIVE;
                break;
            
            case 'h':
                printf("Usage: %s [-hlv] [-d] HOST PORT\n", argv[0]);
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

    if ((argc - optind) < 2) {
        fprintf(stderr, "Please give HOST and PORT\n");
        exit(EXIT_FAILURE);
    }

    if ((port = strtol(argv[optind + 1], &t, 0)) == 0) {
        fprintf(stderr, "Please give HOST and PORT\n");
        exit(EXIT_FAILURE);
    }

    if (mode == MODE_PASSIVE && daemon(0, 0) < 0) {
        perror("Palantir error");
        exit(EXIT_FAILURE);            
    }

    atexit(palantir_exit);

    if (palantir_start(argv[optind], (uint16_t)port) < 0) {
        perror("Palantir error");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);        
}
