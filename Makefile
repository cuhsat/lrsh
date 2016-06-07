CC=gcc
LUA=luac5.3
CFLAGS=-c -ansi -pedantic -std=gnu99 -Wall -Werror
LDFLAGS=-llua5.3
SOURCES=src/palantir.c src/lib/lua.c src/lib/net.c
SCRIPTS=src/palantir.lua
OBJECTS=$(SOURCES:.c=.o)
RESOURCES=$(SCRIPTS:.lua=.inc)
EXECUTABLE=palantir

.PHONY: all install clean

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(RESOURCES) $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

%.inc: %.lua
	$(LUA) -s -o $<c $< && xxd -i $<c $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

install:
	cp $(EXECUTABLE) /usr/bin/

clean:
	-@find . -regex ".*\.\(luac\|inc\|o\)" -delete
	-@rm -f $(EXECUTABLE)
