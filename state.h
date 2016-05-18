#ifndef _STATE_H
#define _STATE_H

#include <stdio.h>
#include <stdlib.h>

#include "dbg.h"

#define BOARD_DIM 19
#define BOARD_SIZE BOARD_DIM*BOARD_DIM  // Should I just say 361?

#define NUM_NEIGHBORS 4

#define STATE_WHITE 1
#define STATE_BLACK -1
#define STATE_EMPTY 0
#define STATE_TRAVERSED -3

#define STATE_ALL -2

// Encodes all necessary board state
typedef struct {
	int board[BOARD_SIZE];
	int turn;  // 1 for white, -1 for black  (ugh, this is going to screw me, first turn is different than chess ^^^)
	int koPoint;  // The place (if any, on the board that is the ko point.  -1 otherwise)
	int whitePrisoners;  // The number of white stones that have been captured (by black)
	int blackPrisoners;  // Vice versa
} State;

// An array of specified neighbors to a point
typedef struct {
	int array[NUM_NEIGHBORS];
	int count;
} Neighbors;

// The array of valid moves
typedef struct {
	int array[BOARD_SIZE];
	int count;
} Moves;

/*
// Records what position a move took place, turn, ko point, and neighbors that got killed (everything else can be extrapolated)
typedef struct {
	int point;
	int turn;
	int koPoint;
	Neighbors neighbors;
} MoveRecord;
*/

// Allocates a new state struct, initially empty, black to move
State *createState(void);

// Destroys state
int destroyState(State *state);

// Displays state in ASCII text
void displayState(State *state);

// Sees if a move is legal given the current state
// Checks three things: unoccupied, ko kosher, and not suicidal
int isLegalMove(State *state, int move);

// Sees whether the family of stones that the point is connected to has a liberty
// Returns 0 if no stone at the point
int hasLiberties(State *state, int point);

// Recursively sets empty all the neighbors of the point matching the same type
// Obviously, assumes hasLiberties was false
// Returns number of points set to empty
int setEmpty(State *state, int point);

// Gets all the neighbors of point that are of type (-1, 0, 1, for black, empty, white respectively), actually, see defined variables above
// If -2 is passed, then all match.
Neighbors getNeighborsOfType(State *state, int point, int type);

// Makes the move according to the given state.  Input state will be changed.
// Assumes valid move
void makeMove(State *state, int move);

// Returns all valid moves (pretty trivial, actually)
Moves getMoves(State *state);


#endif