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
#include "lua.h"
#include "../lib/net.h"
#include "../lib/sys.h"

#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*
 * Lua connect
 */
extern int lua_connect(lua_State *L) {
    host_t host = {
        luaL_checkstring(L, 1),
        luaL_checkinteger(L, 2)
    };

    if (net_connect(host) < 0) {
        return luaL_error(L, strerror(errno));
    }

    return 0;
}

/*
 * Lua listen
 */
extern int lua_listen(lua_State *L) {
    host_t host = {
        luaL_checkstring(L, 1),
        luaL_checkinteger(L, 2)
    };

    if (net_listen(host) < 0) {
        return luaL_error(L, strerror(errno));
    }

    return 0;
}

/*
 * Lua accept
 */
extern int lua_accept(lua_State *L) {
    if (net_accept() < 0) {
        return luaL_error(L, strerror(errno));
    }

    return 0;
}

/*
 * Lua send
 */
extern int lua_send(lua_State *L) {
    frame_t frame; size_t s1 = 0, s2 = 0;

    const char *p1 = luaL_checklstring(L, 1, &s1);
    const char *p2 = luaL_optlstring(L, 2, "", &s2);

    frame.size = s1 + 1 + s2;

    if ((frame.data = (char *)malloc(frame.size)) == NULL) {
        return luaL_error(L, strerror(errno));
    }

    memcpy(frame.data, p1, s1);
    memcpy(frame.data + s1, " ", 1);
    memcpy(frame.data + s1 + 1, p2, s2);

    if (net_send(frame) < 0) {
        return luaL_error(L, strerror(errno));
    }

    free(frame.data);

    return 0;
}

/*
 * Lua recv
 */
extern int lua_recv(lua_State *L) {
    frame_t frame; char *p;

    if (net_recv(&frame) < 0) {
        return luaL_error(L, strerror(errno));
    }    

    if ((p = memchr(frame.data, ' ', frame.size)) != NULL) {
        lua_pushlstring(L, frame.data, p - frame.data);
        lua_pushlstring(L, p + 1, frame.size - (1 + (p - frame.data)));
    } else {
        lua_pushlstring(L, frame.data, frame.size);
        lua_pushnil(L);
    }

    return 2;
}

/*
 * Lua handler
 */
extern int lua_handler(lua_State *L) {
    sys_handler(luaL_optstring(L, 1, "unknown"));

    return 0;
}

/*
 * Lua prompt
 */
extern int lua_prompt(lua_State *L) {
    prompt_t prompt = {
        luaL_checkstring(L, 1)
    };

    if (sys_prompt(&prompt) < 0) {
        return luaL_error(L, strerror(errno));
    }
    
    lua_pushlstring(L, prompt.line, strnlen(prompt.line, sizeof(prompt.line)));

    return 1;
}

/*
 * Lua sleep
 */
extern int lua_sleep(lua_State *L) {
    sys_sleep(luaL_checkinteger(L, 1));

    return 0;
}

/*
 * Lua info
 */
extern int lua_info(lua_State *L) {
    info_t info;

    if (lua_gettop(L) > 0) {
        strcpy(info.path, luaL_checkstring(L, 1));
    } else {
        memset(info.path, 0, sizeof(info.path));
    }

    if (sys_info(&info)) {
        return luaL_error(L, strerror(errno));
    }

    lua_pushlstring(L, info.user, strnlen(info.user, sizeof(info.user)));
    lua_pushlstring(L, info.host, strnlen(info.host, sizeof(info.host)));
    lua_pushlstring(L, info.path, strnlen(info.path, sizeof(info.path)));

    return 3;
}
