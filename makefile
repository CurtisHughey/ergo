# Makefile

CC = gcc-4.9
LFLAGS = -std=gnu99
CFLAGS = -pthread -Wall -Werror -Wno-array-bounds
DIM = -D BOARD_DIM=19
OPTLEVEL = -O3
VALGRIND =
SRC = $(wildcard *.c)
OBJECTS = $(SRC:.c=.o)
EXE = ergo

EXE: $(OBJECTS)
	$(CC) $(LFLAGS) $(CFLAGS) $(OPTLEVEL) $(VALGRIND) $(OBJECTS) -o $(EXE) -lm

%.o: %.c
	$(CC) $(DIM) $(LFLAGS) $(CFLAGS) $(OPTLEVEL) $(VALGRIND) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXE)
