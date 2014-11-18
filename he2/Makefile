# Let's define some variables
CLIENT_NAME :=client
SERVER_NAME :=server
CLIENT_OBJS :=client.o
SERVER_OBJS :=server.o
CC = gcc

# Release preset
#CFLAGS=-O3 -Wall -std=c99
# Debug preset
CFLAGS=-O0 -g3 -Wall -Wextra -std=c99 -DDEBUG -D_POSIX_C_SOURCE=200112L

# Declare "phony targets"
.PHONY: all clean

all: $(SERVER_NAME) $(CLIENT NAME)

$(SERVER_NAME): $(SERVER_OBJS)
	$(CC) -o $(SERVER_NAME) $(SERVER_OBJS)

$(CLIENT_NAME): $(CLIENT_OBJS)
	$(CC) -o $(CLIENT_NAME) $(CLIENT_OBJS)

# '@' Runs it in silent mode, and '-' will supress errors.
clean:
	@- $(RM) $(CLIENT_NAME)
	@- $(RM) $(SERVER_NAME)
	@- $(RM) $(CLIENT_OBJS)
	@- $(RM) $(SERVER_OBJS)
