CC=gcc
LUA=luac5.3
CFLAGS=-c -ansi -pedantic -std=gnu99 -Wall -Werror -DREADLINE
LDFLAGS=-lreadline -llua5.3
SOURCES=$(shell find src -name '*.c')
SCRIPTS=$(shell find src -name '*.lua')
OBJECTS=$(SOURCES:.c=.o)
RESOURCES=$(SCRIPTS:.lua=.inc)
EXECUTABLE=palantir

.PHONY: all install remove clean test

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(RESOURCES) $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

%.inc: %.lua
	$(LUA) -s -o $<c $< && xxd -i $<c $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

install:
	gzip -c doc/$(EXECUTABLE).man > /usr/share/man/man1/$(EXECUTABLE).1.gz
	cp $(EXECUTABLE) /usr/bin/

remove:
	rm /usr/share/man/man1/$(EXECUTABLE).1.gz
	rm /usr/bin/$(EXECUTABLE)

clean:
	-@find . -regex ".*\.\(luac\|inc\|o\)" -delete
	-@rm -f $(EXECUTABLE)

test: all
	-@./test/test.sh
