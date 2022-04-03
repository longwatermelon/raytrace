CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Werror -pedantic -O3
LIBS=-lm -lpthread

SRC=$(wildcard src/*.c)
OBJS=$(addprefix obj/, $(SRC:.c=.o))

all:
	mkdir -p obj/src
	$(MAKE) raytrace

raytrace: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

obj/src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

clean:
	-rm -rf obj/* raytrace
