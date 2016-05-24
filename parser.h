#ifndef _PARSER_H
#define _PARSER_H

#include <stdio.h>

#include "state.h"
#include "dbg.h"

#define MAX_MOVE_LEN 5  // Ish

// Parses a custom board in a file into a State
// Returns NULL if incorrectly formatted
State *parseState(char *fileName);

// Writes state to file.  Inverse of parseState
void serializeState(State *state, char *fileName);

// Parses a move.  Returns -2 if incorrectly formatted (-1 means pass)
int parseMove(char *fileName);

#endif