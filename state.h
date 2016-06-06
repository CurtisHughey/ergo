#ifndef _STATE_H
#define _STATE_H

#include <stdio.h>
#include <stdlib.h>

#include "dbg.h"

#define BOARD_DIM 19
#define BOARD_SIZE BOARD_DIM*BOARD_DIM  // Should I just say 361?

#define NUM_NEIGHBORS 4

// Maybe change these to EMPTY_*.  Realllly should change this to enum
#define STATE_WHITE 1
#define STATE_BLACK (-1)
#define STATE_EMPTY 0
#define STATE_TRAVERSED (-3)
#define STATE_YES (-4)
#define STATE_NO (-5)

#define STATE_ALL (-2)

#define MOVE_PASS (-1)

#define OTHER_COLOR(x) ((-1)*x)

// Encodes all necessary board state
typedef struct {
	int board[BOARD_SIZE];
	int turn;  // 1 for white, -1 for black  (ugh, this is going to screw me, first turn is different than chess ^^^)
	int koPoint;  // The place (if any, on the board that is the ko point.  -1 otherwise)
	int whitePrisoners;  // The number of white stones that have been captured (by black)
	int blackPrisoners;  // Vice versa
	int blackPassed;
} State;

// An array of specified neighbors (left, right, up, down) to a point
typedef struct {
	int array[NUM_NEIGHBORS];
	int count;
} Neighbors;

// The array of valid moves
typedef struct {
	int array[BOARD_SIZE+1];  // +1 for MOVE_PASS
	int count;
} Moves;

// THe info needed to undo a move
typedef struct {
	int move;  // Previous location (-1 if pass)
	Neighbors needToFill;  // The neighbors that need to get filled with the opposite color
	int koPoint;  // Previous ko point
} UnmakeMoveInfo;

// The score for both players
typedef struct {
	int whiteScore;
	int blackScore;
} Score;

// Allocates a new state struct, initially empty, black to move
State *createState(void);

// Destroys state
int destroyState(State *state);

// Copies state
State *copyState(State *original);

// Displays state in ASCII text
void displayState(State *state);

// Sees if a move is legal given the current state
// Checks three things: unoccupied, ko kosher, and not suicidal
int isLegalMove(State *state, int move);

// Sees whether the family of stones that the point is connected to the type provided
// If type==STATE_EMPTY, then this sees if there are liberties
// This function now marks STATE_TRAVERSED (so you'd need to reset, ugh)
int groupBordersType(State *state, int point, int type);

// Wrapper function, calls groupBordersType and then resets STATE_TRAVERSED
int groupBordersTypeAndReset(State *state, int point, int type);

// Recursively sets to type all the neighbors of the point matching the same type
// Returns number of points set
int fillWith(State *state, int point, int type);

// Sets all empty connected to STATE_YES if it is territory for color, otherwise STATE_NO 
int setTerritory(State *state, int point, int color);

// Gets all the neighbors of point that are of type (-1, 0, 1, for black, empty, white respectively), actually, see defined variables above
// If -2 is passed, then all match.
void getNeighborsOfType(State *state, int point, int type, Neighbors *neighbors);

// Makes the move according to the given state.  Input state will be changed.
// Assumes valid move
void makeMove(State *state, int move);

// Makes the move and saves the previous info
void makeMoveAndSave(State *state, int move, UnmakeMoveInfo *unmakeMoveInfo);

// Makes move according to the given state, returns info needed to undo move
void unmakeMove(State *state, UnmakeMoveInfo *unmakeMoveInfo);

// Returns all valid moves (pretty trivial, actually)
// Pass move is always stored last (as -1)
Moves *getMoves(State *state);

// Calculates the score for the given type according to Chinese rules:
// Living stones + territory
// Note only one score is returned (see below)
int calcScore(State *state, int type);

// Returns both scores
Score calcScores(State *state);

// Returns 1 if the states are equal, 0 otherwise
int statesAreEqual(State *state1, State *state2);

#endif