#ifndef _STATE_H
#define _STATE_H

#include <stdio.h>
#include <stdlib.h>

#include "dbg.h"

#define NUM_NEIGHBORS 4

#define STATE_WHITE 1
#define STATE_BLACK -1
#define STATE_EMPTY 0
#define STATE_TRAVERSED -3

#define STATE_ALL -2

// Encodes all necessary board state
typedef struct {
	int boardDim;
	int boardSize;  // Should be boardDim^2
	int *board;
	int turn;  // 1 for white, -1 for black  (ugh, this is going to screw me, first turn is different than chess ^^^)
	int koPoint;  // The place (if any, on the board that is the ko point.  -1 otherwise)
} State;

typedef struct {
	int array[NUM_NEIGHBORS];
	int count;
} Neighbors;

// Allocates a new state struct, initially empty, black to move
State *createState(int boardDim);

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

// Gets all the neighbors of point that are of type (-1, 0, 1, for black, empty, white respectively), actually, see defined variables above
// If -2 is passed, then all match
Neighbors getNeighborsOfType(State *state, int point, int type);

// Makes the move according to the given state.  Input state will be changed, and the state from the previous position is returned
// Assumes valid move
State *makeMove(State *state, int move);



#endif