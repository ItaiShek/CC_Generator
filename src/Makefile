# create an executable for CC_Generator

NAME=CC_Generator

# user gcc as compiler
CC=gcc
# linker flags - math
LFLAGS=-lm
# dependencies
DEPS = generatefuncs.h graphics.h sighandler.h userfuncs.h

OBJ = generatefuncs.o graphics.o sighandler.o userfuncs.o main.o

all: $(NAME)

%.o: %.c $(DEPS)
	$(CC) -c $< -o $@ 

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(LFLAGS)
	rm -f $(OBJ) 2> /dev/null

.PHONY: clean

clean:
	rm -f CC_Generator *.o
