CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11
LIBS = -lcurl -ljson-c
INCLUDES = -I/usr/include/json-c

OBJS = main.o lib.o

all: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LIBS) -o res

main.o: main.c lib.h
	$(CC) $(CFLAGS) $(INCLUDES) -c main.c

lib.o: lib.c lib.h
	$(CC) $(CFLAGS) -c lib.c

clean:
	rm -f $(OBJS) res
