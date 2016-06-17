#ifndef _GAMERUNNER_H
#define _GAMERUNNER_H

#include <stdio.h>
#include <time.h>

#include "state.h"
#include "parser.h"
#include "uct.h"

// Prints the results of a game
void showResults(State *state);

// Prompts human for valid move and does it, returns 1 if the game has finished
int promptHuman(State *state, char *color);

void runHumanVsHuman(void);

void runHumanVsComputer(void);

void runComputerVsComputer(void);

// Used for testing
void runComputerVsRandom(void);
#endif