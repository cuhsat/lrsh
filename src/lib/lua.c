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
#include "net.h"

#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*
 * Lua connect
 */
extern int lua_connect(lua_State *L) {
    if (!lua_isstring(L, 1) || !lua_isinteger(L, 2)) {
        return luaL_error(L, "invalid args");
    }

    if (net_connect(lua_tostring(L, 1), lua_tointeger(L, 2)) < 0) {
        return luaL_error(L, strerror(errno));
    }

    return 0;
}

/*
 * Lua listen
 */
extern int lua_listen(lua_State *L) {
    if (!lua_isstring(L, 1) || !lua_isinteger(L, 2)) {
        return luaL_error(L, "invalid args");
    }

    if (net_listen(lua_tostring(L, 1), lua_tointeger(L, 2)) < 0) {
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
    if (!lua_isstring(L, 1)) {
        return luaL_error(L, "invalid args");
    }

    size_t size = 0;
    const char *data = lua_tolstring(L, 1, &size);

    if (net_send(data, size) < 0) {
        return luaL_error(L, strerror(errno));
    }

    return 0;
}

/*
 * Lua recv
 */
extern int lua_recv(lua_State *L) {
    char *data = NULL;
    size_t size = 0;

    if (net_recv(&data, &size) < 0) {
        return luaL_error(L, strerror(errno));
    }

    lua_pushlstring(L, data, size);

    return 1;
}

/*
 * Lua sleep
 */
extern int lua_sleep(lua_State *L) {
    if (!lua_isinteger(L, 1)) {
        return luaL_error(L, "invalid args");
    }

    time_t time = lua_tointeger(L, 1);
    struct timespec ts;

    ts.tv_sec  = (time / 1000);
    ts.tv_nsec = (time % 1000) * 1000000L;

    while (nanosleep(&ts, &ts) < 0) {
        continue;
    }

    return 0;
}

/*
 * Lua system
 */
extern int lua_system(lua_State *L) {
    if (lua_gettop(L) > 0) {
        if (!lua_isstring(L, 1)) {
            return luaL_error(L, "invalid args");
        }

        if (chdir(lua_tostring(L, 1)) < 0) {
            return luaL_error(L, strerror(errno));
        }
    }    

    char user[LOGIN_NAME_MAX + 1];
    char host[HOST_NAME_MAX + 1];
    char path[PATH_MAX + 1];

    if (getlogin_r(user, sizeof(user)) < 0) {
        return luaL_error(L, strerror(errno));
    }

    if (gethostname(host, sizeof(host)) < 0) {
        return luaL_error(L, strerror(errno));   
    }

    if (getcwd(path, sizeof(path)) == NULL) {
        return luaL_error(L, strerror(errno));
    }

    lua_pushlstring(L, user, strnlen(user, sizeof(user)));
    lua_pushlstring(L, host, strnlen(host, sizeof(host)));
    lua_pushlstring(L, path, strnlen(path, sizeof(path)));

    return 3;
}
