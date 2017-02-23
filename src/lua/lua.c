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
#include "lua.h"
#include "../lib/net.h"
#include "../lib/os.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#if (defined(DEBUG) && (DEBUG == 1))
#define LUA_TRACE() fprintf(stderr, "-- %s\n", __func__)
#else
#define LUA_TRACE()
#endif

/**
 * Lua panic
 * @param L the Lua state address
 * @return stack count
 */
extern int lua_panic(lua_State *L) {
    fprintf(stderr, "Palantir panic: %s\n", lua_tostring(L, -1));

    return 0;
}

/**
 * Lua connect
 * @param L the Lua state address
 * @return stack count
 */
extern int lua_connect(lua_State *L) {
    LUA_TRACE();

    host_t host = {
        luaL_checkstring(L, 1),
        (const uint16_t)luaL_checkinteger(L, 2)
    };

    if (net_connect(&host) < 0) {
        return luaL_error(L, strerror(errno));
    }

    return 0;
}

/**
 * Lua listen
 * @param L the Lua state address
 * @return stack count
 */
extern int lua_listen(lua_State *L) {
    LUA_TRACE();

    host_t host = {
        luaL_checkstring(L, 1),
        (const uint16_t)luaL_checkinteger(L, 2)
    };

    if (net_listen(&host) < 0) {
        return luaL_error(L, strerror(errno));
    }

    return 0;
}

/**
 * Lua accept
 * @param L the Lua state address
 * @return stack count
 */
extern int lua_accept(lua_State *L) {
    LUA_TRACE();

    if (net_accept() < 0) {
        return luaL_error(L, strerror(errno));
    }

    return 0;
}

/**
 * Lua send
 * @param L the Lua state address
 * @return stack count
 */
extern int lua_send(lua_State *L) {
    LUA_TRACE();

    frame_t frame;

    frame.data = (char *)luaL_checklstring(L, 1, &(frame.size));

    if (net_send(&frame) < 0) {
        return luaL_error(L, strerror(errno));
    }

    return 0;
}

/**
 * Lua recv
 * @param L the Lua state address
 * @return stack count
 */
extern int lua_recv(lua_State *L) {
    LUA_TRACE();

    frame_t frame;

    if (net_recv(&frame) < 0) {
        return luaL_error(L, strerror(errno));
    }

    lua_pushlstring(L, frame.data, frame.size);

    return 1;
}

/**
 * Lua prompt
 * @param L the Lua state address
 * @return stack count
 */
extern int lua_prompt(lua_State *L) {
    LUA_TRACE();

    prompt_t prompt = {
        luaL_checkstring(L, 1)
    };

    if (os_prompt(&prompt) < 0) {
        return luaL_error(L, strerror(errno));
    }

    lua_pushlstring(L, prompt.line, strlen(prompt.line));

    return 1;
}

/**
 * Lua sleep
 * @param L the Lua state address
 * @return stack count
 */
extern int lua_sleep(lua_State *L) {
    LUA_TRACE();

    os_sleep((time_t)luaL_checkinteger(L, 1));

    return 0;
}

/**
 * Lua env
 * @param L the Lua state address
 * @return stack count
 */
extern int lua_env(lua_State *L) {
    LUA_TRACE();

    env_t env;

    if (lua_gettop(L) > 0) {
        strcpy(env.path, luaL_checkstring(L, 1));
    } else {
        memset(env.path, 0, sizeof(env.path));
    }

    if (os_env(&env)) {
        return luaL_error(L, strerror(errno));
    }

    lua_pushlstring(L, env.user, strlen(env.user));
    lua_pushlstring(L, env.host, strlen(env.host));
    lua_pushlstring(L, env.path, strlen(env.path));

    return 3;
}
