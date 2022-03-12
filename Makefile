CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Werror -pedantic -O3
LIBS=-lm -lpthread

SRC=$(wildcard src/*.c)
OBJS=$(notdir $(SRC:.c=.o))

raytrace: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

clean:
	-rm *.o raytrace
