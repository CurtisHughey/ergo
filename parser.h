#ifndef _PARSER_H
#define _PARSER_H

#include <stdio.h>

#include "state.h"
#include "dbg.h"

// Parses a custom board in a file into a State
// Returns NULL if incorrectly formatted
State *parseState(char *fileName);

// Writes state to file.  Inverse of parseState
void serializeState(State *state, char *fileName);

#endif