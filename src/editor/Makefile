CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Werror -pedantic
INC=-Isrc
LIBS=-Llib -lcore -lm -lpthread -lpng -lSDL2

SRC=$(wildcard src/editor/*.c)
OBJS=$(addprefix obj/, $(SRC:.c=.o))

editor: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(INC) $(LIBS)

obj/src/editor/%.o: src/editor/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INC) $(LIBS)

