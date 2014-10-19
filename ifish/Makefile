# Let's define some variables
NAME :=ifish
SRCS := $(wildcard *.c)
#OBJS := $(wildcard *.o)
OBJS =main.o safefork.o
CC = gcc
CFLAGS=-O3 -Wall -std=c99 -DDEBUG

# Declare "phony targets"
.PHONY: all clean distclean

all: ifish

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS)

#$(OBJS): $(SRCS)
#	$(CC) -c -o $(CFLAGS)


#$(NAME): $(OBJ)
#	$(CC) $(CFLAGS) -c $(OBJ) -o $(NAME) -I
#	$(CC) -o $(NAME) $(OBJS)
	
# This target removes the built program and its object files.
# '@' Runs it in silent mode, and '-' will supress errors. 
clean:
	@- $(RM) $(NAME)
	@- $(RM) $(OBJS)

distclean: clean
