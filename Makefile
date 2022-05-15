CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Werror -pedantic -O3
LIBS=-lm -lpthread -lpng

all: animate

core:
	mkdir -p obj/src/core
	$(MAKE) -f src/core/Makefile

cli: core
	mkdir -p obj/src/cli
	$(MAKE) -f src/cli/Makefile

animate: core
	mkdir -p obj/src/animate
	$(MAKE) -f src/animate/Makefile

circle: tools/circle.c
	$(CC) $< -o $@ -lm

clean:
	-rm -rf obj/* lib/* raytrace
