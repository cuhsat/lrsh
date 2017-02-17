cmake_minimum_required(VERSION 3.0)

#
#  Imports
#

include(CheckFunctionExists)
include(CheckLibraryExists)

#
#  Project
#

project(palantir)

#
#  Prerequisites
#

find_package(Lua REQUIRED)
find_program(LUA_EXECUTABLE NAMES lua)
find_program(LUAC_EXECUTABLE NAMES luac)

if(NOT LUA_EXECUTABLE)
    message(FATAL_ERROR "Lua not found!")
endif()

if(NOT LUAC_EXECUTABLE)
    message(FATAL_ERROR "Luac not found!")
endif()

#
#  Source files
#

set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/lib/args.c
    ${CMAKE_SOURCE_DIR}/src/lib/args.h
    ${CMAKE_SOURCE_DIR}/src/lib/crc.c
    ${CMAKE_SOURCE_DIR}/src/lib/crc.h
    ${CMAKE_SOURCE_DIR}/src/lib/dict.h
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

set(LIBRARIES dl)

#
#  Debug mode
#

option(DEBUG "Debug mode" OFF)

if(DEBUG)
    set(CMAKE_VERBOSE_MAKEFILE on)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBUILD='\"${CMAKE_SYSTEM} DEBUG\"'")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG=1")
endif(DEBUG)

#
#  POSIX support
#

if(UNIX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -pedantic -Wall -Werror")
    set(SOURCE_FILES "${SOURCE_FILES}"
        ${CMAKE_SOURCE_DIR}/src/lib/os_posix.c
    )
endif(UNIX)

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
        ${CMAKE_SOURCE_DIR}/src/lib/readline.c
        ${CMAKE_SOURCE_DIR}/src/lib/readline.h
    )
    set(LIBRARIES "${LIBRARIES}" readline)
endif(HAVE_READLINE AND NOT APPLE)

#
#  Includes
#

include_directories(${LUA_INCLUDE_DIR})

#
#  Binary
#

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#
#  Resources
#

add_custom_target(resources
    COMMAND ${LUAC_EXECUTABLE} -s -o palantir.luac ${CMAKE_SOURCE_DIR}/src/palantir.lua
    COMMAND ${LUA_EXECUTABLE} xxd.lua palantir.luac > palantir.inc
)

add_dependencies(${PROJECT_NAME} resources)

#
#  Linking
#

target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES} ${LIBRARIES})

#
#  Testing
#

enable_testing()

add_test(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/test/test.sh)

#
#  Install
#

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
