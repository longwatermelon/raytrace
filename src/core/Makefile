SRC=$(wildcard src/core/*.c)
OBJS=$(addprefix obj/, $(SRC:.c=.o))

CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Werror -pedantic
LIBS=-lm -lpthread -lpng
AR=ar
ARFLAGS=rc

obj/src/core/%.o: src/core/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

libcore.a: $(OBJS)
	mkdir -p lib
	$(AR) $(ARFLAGS) lib/$@ $(OBJS)

