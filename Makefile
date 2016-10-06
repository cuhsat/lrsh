CC=gcc
LC=luac
override CFLAGS+=-c -ansi -pedantic -std=gnu99 -Wall -Werror
override LDFLAGS+=-llua -lm -ldl
SOURCES=$(shell find src -name '*.c')
SCRIPTS=$(shell find src -name '*.lua')
OBJECTS=$(SOURCES:.c=.o)
RESOURCES=$(SCRIPTS:.lua=.inc)
EXECUTABLE=palantir

ifneq (1,$(NO_READLINE))
override CFLAGS+=-DREADLINE
override LDFLAGS+=-lreadline
endif

.PHONY: all install remove clean test

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(RESOURCES) $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

%.inc: %.lua
	$(LC) -s -o $<c $< && xxd -i $<c $@

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

test:
	-@./test/test.sh $(HOST) $(PORT)
