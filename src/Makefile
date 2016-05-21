CC=gcc
CFLAGS=-c -ansi -pedantic -std=gnu99 -Wall -Werror -DDEBUG
LDFLAGS=-llua5.3
LUA=luac5.3
SOURCES=palantir.c lib/lua.c lib/net.c
SCRIPTS=palantir.lua
OBJECTS=$(SOURCES:.c=.o)
RESOURCES=$(SCRIPTS:.lua=.inc)
EXECUTABLES=../bin
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
	rm -f $(EXECUTABLE) *.luac *.inc *.o lib/*.o
	rmdir $(EXECUTABLES)
