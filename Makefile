CC = gcc
LUA = lua
LUAC = luac
SOURCES = $(shell find src -name '*.c')
SCRIPTS = $(shell find src -name '*.lua')
OBJECTS = $(SOURCES:.c=.o)
RESOURCES = $(SCRIPTS:.lua=.inc)
EXECUTABLE = palantir

override CFLAGS  += -c -std=gnu99 -pedantic -Wall -Werror
override LDFLAGS += -llua -lm -ldl

ifneq (1,$(NO_DAEMON))
override CFLAGS += -DDAEMON
endif

ifneq (1,$(NO_READLINE))
override CFLAGS  += -DREADLINE
override LDFLAGS += -lreadline
else
SOURCES := $(filter-out src/lib/readline.c, ${SOURCES})
OBJECTS := $(filter-out src/lib/readline.o, ${OBJECTS})
endif

.PHONY: all install remove clean test

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(RESOURCES) $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

%.inc: %.lua
	$(LUAC) -s -o $<c $< && $(LUA) build/xxd.lua $<c > $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

install:
	gzip -c doc/$(EXECUTABLE).man > /usr/share/man/man1/$(EXECUTABLE).1.gz
	cp $(EXECUTABLE) /usr/bin/

remove:
	rm /usr/share/man/man1/$(EXECUTABLE).1.gz
	rm /usr/bin/$(EXECUTABLE)

clean:
	-@find src -regex ".*\.luac" -delete
	-@find src -regex ".*\.inc" -delete
	-@find src -regex ".*\.o" -delete
	-@rm -f $(EXECUTABLE)

test:
	-@./test/test.sh $(HOST) $(PORT)
