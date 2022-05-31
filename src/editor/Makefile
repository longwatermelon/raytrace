CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Werror -pedantic
INC=-Isrc
LIBS=-Llib -lcore -lm -lpthread -lpng -lSDL2 -lSDL2_ttf -lSDL2_image

SRC=$(wildcard src/editor/*.c)
OBJS=$(addprefix obj/, $(SRC:.c=.o))

editor: $(OBJS)
	$(CC) $(CFLAGS) $^ $(INC) $(LIBS)

obj/src/editor/%.o: src/editor/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INC) $(LIBS)

