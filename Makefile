CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Werror -pedantic
LIBS=-lm

SRC=$(wildcard src/*.c)
OBJS=$(notdir $(SRC:.c=.o))

raytrace: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

clean:
	-rm *.o raytrace
