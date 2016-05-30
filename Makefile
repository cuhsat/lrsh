CC=gcc
CFLAGS=-c -ansi -pedantic -std=gnu99 -Wall -Werror -DDEBUG=0
LDFLAGS=-llua5.3
LUA=luac5.3
SOURCES=src/palantir.c src/lib/lua.c src/lib/net.c
SCRIPTS=src/palantir.lua
OBJECTS=$(SOURCES:.c=.o)
RESOURCES=$(SCRIPTS:.lua=.inc)
EXECUTABLES=bin
EXECUTABLE=$(EXECUTABLES)/palantir

.PHONY: all clean

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(RESOURCES) $(OBJECTS)
	@mkdir -p $(EXECUTABLES)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE) 

%.inc: %.lua
	$(LUA) -s -o $<c $< && xxd -i $<c $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-@find . -regex ".*\.\(luac\|inc\|o\)" -delete
	-@rm -f $(EXECUTABLE)
	-@rmdir $(EXECUTABLES)
