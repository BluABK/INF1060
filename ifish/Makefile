# Let's define some variables
NAME :=ifish
OBJS =main.o safefork.o
CC = gcc

# Release preset
CFLAGS=-O3 -Wall -std=c99
# Debug preset
# CFLAGS=-O0 -g3 -Wall -Wextra -std=c99 -DDEBUG

# Declare "phony targets"
.PHONY: all clean

all: ifish

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS)

# '@' Runs it in silent mode, and '-' will supress errors.
clean:
	@- $(RM) $(NAME)
	@- $(RM) $(OBJS)
