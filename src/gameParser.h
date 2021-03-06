#ifndef _GAMEPARSER_H
#define _GAMEPARSER_H

#include <stdio.h>

#include "state.h"
#include "dbg.h"

#define MAX_BOARD_LINE 1024  // Allows for comments (really shouldn't be more than this on one line :) )
#define MAX_MOVE_LEN 7  // Ish

// Parses a custom board in a file into a State
// Returns NULL if incorrectly formatted
State *parseState(char *fileName);

// Writes state to file.  Inverse of parseState if append==0.  If append==1, then the game is added to end of file
void serializeState(State *state, char *fileName, int append);

// Parses a move from a file
int parseMoveFromFile(char *fileName);

// Parses a move from stdin
int parseMoveFromTerminal(void);

// Parses a move (internal function).  Returns -2 if incorrectly formatted (-1 means pass)
int parseMove(char *line);

// Translates int move into string
char *moveToString(int move, int color);

#endif