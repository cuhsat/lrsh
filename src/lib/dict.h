/**
 * Copyright (c) 2016-2017 Christian Uhsat <christian@uhsat.de>
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software")
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
#ifndef LIB_DICT_H
#define LIB_DICT_H

#include <lua.h>

#include <stddef.h>

#if LUA_VERSION_NUM >= 503
#define LUA_53(s) (s),
#else
#define LUA_53(s)
#endif

#if LUA_VERSION_NUM >= 502
#define LUA_52(s) (s),
#else
#define LUA_52(s)
#endif

#if LUA_VERSION_NUM >= 501
#define LUA_51(s) (s),
#else
#define LUA_51(s)
#endif

const char *dict[] = {

    // Lua keywords
    LUA_51("and")
    LUA_51("break")
    LUA_51("do")
    LUA_51("else")
    LUA_51("elseif")
    LUA_51("end")
    LUA_51("false")
    LUA_51("for")
    LUA_51("function")
    LUA_52("goto")
    LUA_51("if")
    LUA_51("in")
    LUA_51("local")
    LUA_51("nil")
    LUA_51("not")
    LUA_51("or")
    LUA_51("repeat")
    LUA_51("return")
    LUA_51("then")
    LUA_51("true")
    LUA_51("until")
    LUA_51("while")

    // Lua globals
    LUA_51("_G")
    LUA_51("_VERSION")
    LUA_53("arg")

    // Lua functions
    LUA_51("assert")
    LUA_51("collectgarbage")
    LUA_51("dofile")
    LUA_51("error")
    LUA_51("getmetatable")
    LUA_51("ipairs")
    LUA_51("load")
    LUA_51("loadfile")
    LUA_51("next")
    LUA_51("pairs")
    LUA_51("pcall")
    LUA_51("print")
    LUA_51("rawequal")
    LUA_51("rawget")
    LUA_51("rawlen")
    LUA_51("rawset")
    LUA_51("require")
    LUA_51("select")
    LUA_51("setmetatable")
    LUA_51("tonumber")
    LUA_51("tostring")
    LUA_51("type")
    LUA_51("xpcall")
    LUA_51("coroutine")
    LUA_51("coroutine.create")
    LUA_51("coroutine.isyieldable")
    LUA_51("coroutine.resume")
    LUA_51("coroutine.running")
    LUA_51("coroutine.status")
    LUA_51("coroutine.wrap")
    LUA_51("coroutine.yield")
    LUA_51("debug")
    LUA_51("debug.debug")
    LUA_51("debug.gethook")
    LUA_51("debug.getinfo")
    LUA_51("debug.getlocal")
    LUA_51("debug.getmetatable")
    LUA_51("debug.getregistry")
    LUA_51("debug.getupvalue")
    LUA_51("debug.getuservalue")
    LUA_51("debug.sethook")
    LUA_51("debug.setlocal")
    LUA_51("debug.setmetatable")
    LUA_51("debug.setupvalue")
    LUA_51("debug.setuservalue")
    LUA_51("debug.traceback")
    LUA_51("debug.upvalueid")
    LUA_51("debug.upvaluejoin")
    LUA_51("io")
    LUA_51("io.close")
    LUA_51("io.flush")
    LUA_51("io.input")
    LUA_51("io.lines")
    LUA_51("io.open")
    LUA_51("io.output")
    LUA_51("io.popen")
    LUA_51("io.read")
    LUA_51("io.stderr")
    LUA_51("io.stdin")
    LUA_51("io.stdout")
    LUA_51("io.tmpfile")
    LUA_51("io.type")
    LUA_51("io.write")
    LUA_51("file:close")
    LUA_51("file:flush")
    LUA_51("file:lines")
    LUA_51("file:read")
    LUA_51("file:seek")
    LUA_51("file:setvbuf")
    LUA_51("file:write")
    LUA_51("math")
    LUA_51("math.abs")
    LUA_51("math.acos")
    LUA_51("math.asin")
    LUA_51("math.atan")
    LUA_51("math.ceil")
    LUA_51("math.cos")
    LUA_51("math.deg")
    LUA_51("math.exp")
    LUA_51("math.floor")
    LUA_51("math.fmod")
    LUA_51("math.huge")
    LUA_51("math.log")
    LUA_51("math.max")
    LUA_51("math.maxinteger")
    LUA_51("math.min")
    LUA_51("math.mininteger")
    LUA_51("math.modf")
    LUA_51("math.pi")
    LUA_51("math.rad")
    LUA_51("math.random")
    LUA_51("math.randomseed")
    LUA_51("math.sin")
    LUA_51("math.sqrt")
    LUA_51("math.tan")
    LUA_51("math.tointeger")
    LUA_51("math.type")
    LUA_51("math.ult")
    LUA_51("os")
    LUA_51("os.clock")
    LUA_51("os.date")
    LUA_51("os.difftime")
    LUA_51("os.execute")
    LUA_51("os.exit")
    LUA_51("os.getenv")
    LUA_51("os.remove")
    LUA_51("os.rename")
    LUA_51("os.setlocale")
    LUA_51("os.time")
    LUA_51("os.tmpname")
    LUA_51("package")
    LUA_51("package.config")
    LUA_51("package.cpath")
    LUA_51("package.loaded")
    LUA_51("package.loadlib")
    LUA_51("package.path")
    LUA_51("package.preload")
    LUA_51("package.searchers")
    LUA_52("package.searchpath")
    LUA_51("string")
    LUA_51("string.byte")
    LUA_51("string.char")
    LUA_51("string.dump")
    LUA_51("string.find")
    LUA_51("string.format")
    LUA_51("string.gmatch")
    LUA_51("string.gsub")
    LUA_51("string.len")
    LUA_51("string.lower")
    LUA_51("string.match")
    LUA_53("string.pack")
    LUA_53("string.packsize")
    LUA_51("string.rep")
    LUA_51("string.reverse")
    LUA_51("string.sub")
    LUA_53("string.unpack")
    LUA_51("string.upper")
    LUA_51("table")
    LUA_51("table.concat")
    LUA_51("table.insert")
    LUA_51("table.move")
    LUA_53("table.pack")
    LUA_51("table.remove")
    LUA_51("table.sort")
    LUA_51("table.unpack")
    LUA_53("utf8")
    LUA_53("utf8.char")
    LUA_53("utf8.charpattern")
    LUA_53("utf8.codepoint")
    LUA_53("utf8.codes")
    LUA_53("utf8.len")
    LUA_53("utf8.offset")

    // Own globals
    "BUILD",
    "DEBUG",
    "HOME",
    "HOST",
    "MODE",
    "PORT",
    "TOKEN",
    "VERSION",

    // Own functions
    "net",
    "net.accept",
    "net.client",
    "net.connect",
    "net.listen",
    "net.recv",
    "net.send",
    "net.server",
    "os.path",
    "os.prompt",
    "os.shell",
    "os.sleep",

    // Commands
    "--exit",
    "--halt",

    NULL
};

#endif // LIB_DICT_H
