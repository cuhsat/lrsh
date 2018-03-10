cmake_minimum_required(VERSION 3.0)

#
#  Project
#

project(palantir)

#
#  Settings
#

enable_testing()

#
#  Imports
#

include(CheckFunctionExists)
include(CheckLibraryExists)

#
#  Prerequisites
#

find_package(Lua REQUIRED)
find_program(LUA_EXECUTABLE NAMES lua)
find_program(LUAC_EXECUTABLE NAMES luac)

if(NOT LUA_EXECUTABLE)
    message(FATAL_ERROR "Lua not found!")
endif(NOT LUA_EXECUTABLE)

if(NOT LUAC_EXECUTABLE)
    message(FATAL_ERROR "Luac not found!")
endif(NOT LUAC_EXECUTABLE)

if(NOT UNIX AND NOT WIN32)
    message(FATAL_ERROR "System not supported!")
endif(NOT UNIX AND NOT WIN32)

#
#  Compiler flags
#

set(BUILD "${CMAKE_SYSTEM} ${CMAKE_C_COMPILER_ID}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -pedantic -Wall -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBUILD='\"${BUILD}\"'")

#
#  Source files
#

set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/lib/args.c
    ${CMAKE_SOURCE_DIR}/src/lib/args.h
    ${CMAKE_SOURCE_DIR}/src/lib/crc.c
    ${CMAKE_SOURCE_DIR}/src/lib/crc.h
    ${CMAKE_SOURCE_DIR}/src/lib/net.c
    ${CMAKE_SOURCE_DIR}/src/lib/net.h
    ${CMAKE_SOURCE_DIR}/src/lib/os.h
    ${CMAKE_SOURCE_DIR}/src/lua/lua.c
    ${CMAKE_SOURCE_DIR}/src/lua/lua.h
    ${CMAKE_SOURCE_DIR}/src/palantir.c
    ${CMAKE_SOURCE_DIR}/src/palantir.h
    ${CMAKE_SOURCE_DIR}/src/palantir.lua
)

#
#  Libraries
#

set(LIBRARIES "")

#
#  Debug mode
#

option(DEBUG "Debug mode" OFF)

if(DEBUG)
    set(CMAKE_VERBOSE_MAKEFILE on)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG=1")
endif(DEBUG)

#
#  POSIX support
#

if(UNIX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_DEFAULT_SOURCE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_BSD_SOURCE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPOSIX=1")
    set(SOURCE_FILES "${SOURCE_FILES}"
        ${CMAKE_SOURCE_DIR}/src/lib/os_posix.c
    )
    set(LIBRARIES "${LIBRARIES}" dl)
endif(UNIX)

#
#  Windows NT support
#

if(WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWINNT=1")
    set(SOURCE_FILES "${SOURCE_FILES}"
        ${CMAKE_SOURCE_DIR}/src/lib/os_winnt.c
    )
    set(LIBRARIES "${LIBRARIES}" ws2_32)
endif(WIN32)

#
#  Daemon support (Excluding OS X)
#

check_function_exists(daemon HAVE_DAEMON)

if(HAVE_DAEMON AND NOT APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_DAEMON=1")
endif(HAVE_DAEMON AND NOT APPLE)

#
#  Readline support (Excluding OS X)
#

check_library_exists(readline readline "readline.h" HAVE_READLINE)

if(HAVE_READLINE AND NOT APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_READLINE=1")
    set(SOURCE_FILES "${SOURCE_FILES}"
        ${CMAKE_SOURCE_DIR}/src/lib/dict.h
        ${CMAKE_SOURCE_DIR}/src/lib/readline.c
        ${CMAKE_SOURCE_DIR}/src/lib/readline.h
    )
    set(LIBRARIES "${LIBRARIES}" readline)
endif(HAVE_READLINE AND NOT APPLE)

#
#  Includes
#

include_directories(${LUA_INCLUDE_DIR} ${CMAKE_BINARY_DIR})

#
#  Binary
#

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#
#  Resources
#

set(XXD "${CMAKE_SOURCE_DIR}/src/tools/xxd.lua")
set(PALANTIR_LUA "${CMAKE_SOURCE_DIR}/src/palantir.lua")
set(PALANTIR_LUAC "${CMAKE_BINARY_DIR}/palantir.luac")
set(PALANTIR_INC "${CMAKE_BINARY_DIR}/palantir.inc")

add_custom_target(resources
    COMMAND ${LUAC_EXECUTABLE} -s -o ${PALANTIR_LUAC} ${PALANTIR_LUA}
    COMMAND ${LUA_EXECUTABLE} ${XXD} ${PALANTIR_LUAC} > ${PALANTIR_INC}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_dependencies(${PROJECT_NAME} resources)

#
#  Linking
#

target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES} ${LIBRARIES})

#
#  Testing (POSIX only)
#

if(UNIX AND HAVE_READLINE)
    add_test(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/test/test.sh)
endif(UNIX AND HAVE_READLINE)

#
#  Install (POSIX only)
#

if(UNIX)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif(UNIX)
