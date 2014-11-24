.PHONY: all clean
CC=gcc
CFLAGS=-O0 -Wall -Wextra -g3

all: server client

clean:
	rm -f server client *.o


# Dependency time
server: server.o

client: client.o

server.o: server.c

client.o: client.c
