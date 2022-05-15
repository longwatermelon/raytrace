CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Werror -pedantic
INC=-Isrc
LIBS=-Llib -lcore -lm -lpthread -lpng -lSDL2

SRC=$(wildcard src/animate/*.c)
OBJS=$(addprefix obj/, $(SRC:.c=.o))

animate: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(INC) $(LIBS)

obj/src/animate/%.o: src/animate/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INC) $(LIBS)

