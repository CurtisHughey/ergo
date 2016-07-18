#ifndef _GAMEPARSER_H
#define _GAMEPARSER_H

#include <stdio.h>

#include "state.h"
#include "dbg.h"

#define MAX_MOVE_LEN 7  // Ish
#define QUIT (-3)

// Parses a custom board in a file into a State
// Returns NULL if incorrectly formatted
State *parseState(char *fileName);

// Writes state to file.  Inverse of parseState
void serializeState(State *state, char *fileName);

// Parses a move from a file
int parseMoveFromFile(char *fileName);

// Parses a move from stdin
int parseMoveFromTerminal(void);

// Parses a move (internal function).  Returns -2 if incorrectly formatted (-1 means pass)
int parseMove(char *line);

#endif