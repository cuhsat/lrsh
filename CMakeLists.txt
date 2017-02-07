cmake_minimum_required(VERSION 3.0)
include(CheckFunctionExists)
include(CheckLibraryExists)

project(palantir)

find_package(Lua REQUIRED)

set(LIBRARIES dl)

set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/lib/crc.c
    ${CMAKE_SOURCE_DIR}/src/lib/crc.h
    ${CMAKE_SOURCE_DIR}/src/lib/dict.h
    ${CMAKE_SOURCE_DIR}/src/lib/net.c
    ${CMAKE_SOURCE_DIR}/src/lib/net.h
    ${CMAKE_SOURCE_DIR}/src/lib/os.c
    ${CMAKE_SOURCE_DIR}/src/lib/os.h
    ${CMAKE_SOURCE_DIR}/src/lua/lua.c
    ${CMAKE_SOURCE_DIR}/src/lua/lua.h
    ${CMAKE_SOURCE_DIR}/src/palantir.c
    ${CMAKE_SOURCE_DIR}/src/palantir.h
    ${CMAKE_SOURCE_DIR}/src/palantir.lua
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -pedantic -Wall -Werror")

option(HAVE_DEBUG "Debug mode" OFF)

if(HAVE_DEBUG)
    set(CMAKE_VERBOSE_MAKEFILE on)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG=1")
endif(HAVE_DEBUG)

check_function_exists(daemon HAVE_DAEMON)

if(HAVE_DAEMON)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDAEMON=1")
endif(HAVE_DAEMON)

check_library_exists(readline readline "readline.h" HAVE_READLINE)

if(HAVE_READLINE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DREADLINE=1")
    set(SOURCE_FILES "${SOURCE_FILES}"
        ${CMAKE_SOURCE_DIR}/src/lib/readline.c
        ${CMAKE_SOURCE_DIR}/src/lib/readline.h    
    )
    set(LIBRARIES "${LIBRARIES}" readline)
endif(HAVE_READLINE)

include_directories(${LUA_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_custom_target(resources
    COMMAND luac -s -o palantir.luac ${CMAKE_SOURCE_DIR}/src/palantir.lua
    COMMAND lua xxd.lua palantir.luac > palantir.inc
)

add_dependencies(${PROJECT_NAME} resources)

target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES} ${LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
