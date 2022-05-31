CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Werror -pedantic
LIBS=-lm -lpthread -lpng

all: editor

core:
	mkdir -p obj/src/core
	$(MAKE) -f src/core/Makefile

cli: core
	mkdir -p obj/src/cli
	$(MAKE) -f src/cli/Makefile

editor: core
	mkdir -p obj/src/editor
	$(MAKE) -f src/editor/Makefile

circle: tools/circle.c
	$(CC) $< -o $@ -lm

clean:
	-rm -rf obj/* lib/* a.out
