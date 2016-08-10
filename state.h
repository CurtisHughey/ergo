#ifndef _STATE_H
#define _STATE_H

#include <stdio.h>
#include <stdlib.h>

#include "dbg.h"
#include "hash.h"
#include "stateInfo.h"

// An array of specified neighbors (left, right, up, down) to a point
typedef struct {
	int array[4];  // The 4 directions
	int count;
} Neighbors;

// The array of valid moves
typedef struct {
	int *array;
	int count;  // The current number
	int size;   // THe number allocated
} Moves;

// THe info needed to undo a move
typedef struct {
	int move;  // Previous location (-1 if pass)
	Neighbors needToFill;  // The neighbors that need to get filled with the opposite color
	int koPoint;  // Previous ko point
	int blackPassed;  // Whether black had passed before the prior state
} UnmakeMoveInfo;

// The score for both players
typedef struct {
	double whiteScore;
	double blackScore;
} Score;

// Sets the komi
void setKomi(double komi);

// Allocates a new state struct, initially empty (calling clearBoard), black to move
State *createState();

void clearBoard(State *state);

// Destroys state
int destroyState(State *state);

// Copies state
State *copyState(State *original);

// Displays state in ASCII text
void displayState(State *state);

// Sees if a move is legal given the current state
// Checks three things: unoccupied, ko kosher, and not suicidal
// Now checks a fourth thing: if it violates superko (if hashTable == NULL, then this is ignored)
int isLegalMove(State *state, int move, HashTable *hashTable);

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
void makeMove(State *state, int move, HashTable *hashTable);

// Makes the move and saves the previous info
void makeMoveAndSave(State *state, int move, UnmakeMoveInfo *unmakeMoveInfo, HashTable *hashTable);

// Makes move according to the given state, returns info needed to undo move
void unmakeMove(State *state, UnmakeMoveInfo *unmakeMoveInfo, HashTable *hashTable);

// Creates an empty Moves pointer
Moves *createMoves(void);

// Returns all valid moves (pretty trivial, actually)
// Pass move is always stored last (as -1)
Moves *getMoves(State *state, HashTable *hashTable);

// Call this to free moves
void destroyMoves(Moves *moves);

// Calculates the score for the given type according to Chinese rules:
// Living stones + territory
// This function does NOT factor in komi
int calcScore(State *state, int type);

// Returns both scores (calls calcScore)
// This function factors in komi
Score calcScores(State *state);

// Returns the result for the color (-1 for loss, 1 for win, 0 for draw)
int getResult(State *state, int color);

// Returns 1 if the states are equal, 0 otherwise
int statesAreEqual(State *state1, State *state2);

#endif