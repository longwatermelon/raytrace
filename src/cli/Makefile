CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Werror -pedantic
INC=-Isrc
LIBS=-Llib -lcore -lm -lpthread -lpng

cli: obj/src/cli/main.o
	$(CC) $(CFLAGS) $^ $(INC) $(LIBS)

obj/src/cli/main.o: src/cli/main.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INC) $(LIBS)

