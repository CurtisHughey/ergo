// This is a sort of hacky file that resolves some circular dependencies
// Maybe struct State needs to get renamed, since it no longer belongs in state.h

#ifndef _STATEINFO_H
#define _STATEINFO_H

#include "linkedList.h"

// From state.h
// This can be defined via the makefile
#ifndef BOARD_DIM
#define BOARD_DIM 19  // The default
#endif

#define BOARD_SIZE (BOARD_DIM*BOARD_DIM)

// Maybe change these to EMPTY_*.  Realllly should change this to enum
#define STATE_WHITE 1
#define STATE_BLACK (-1)
#define STATE_EMPTY 0
#define STATE_TRAVERSED (-3)
#define STATE_YES (-4)
#define STATE_NO (-5)

#define STATE_ALL (-2)

#define MOVE_PASS (-1)
#define INVALID_MOVE (-2)  // Moved this from gameParser.h

#define OTHER_COLOR(x) ((-1)*(x))

// From hashTable.h
typedef struct {
	Node ***buckets;
	int numBuckets;
} HashTable;

//////

// Encodes all necessary board state (this is where the big bucks are made)
typedef struct {
	int board[BOARD_SIZE];
	int turn;  // 1 for white, -1 for black  (ugh, this is going to screw me, first turn is different than chess.  Hasn't screwed me yet, suck it)
	int koPoint;  // The place (if any, on the board that is the ko point.  -1 otherwise)
	int whitePrisoners;  // The number of white stones that have been captured (by black)
	int blackPrisoners;  // Vice versa
	int blackPassed;  // non-zero if the turn is white and black just passed
} State;
//////

#endif