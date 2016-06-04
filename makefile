# Makefile

CC = gcc-4.9
LFLAGS = -std=gnu99
CFLAGS = -Wall -Werror -Wno-array-bounds -O3 -g  
# -pthread, also get rid of -g at some point
SRC = $(wildcard *.c)
OBJECTS = $(SRC:.c=.o)
EXE = ergo

EXE: $(OBJECTS)
	$(CC) $(LFLAGS) $(CFLAGS) $(OBJECTS) -o $(EXE) -lm

%.o: %.c
	$(CC) $(LFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXE)
