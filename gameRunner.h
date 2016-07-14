#ifndef _GAMERUNNER_H
#define _GAMERUNNER_H

#include <stdio.h>

#include "state.h"
#include "gameParser.h"
#include "uct.h"
#include "timer.h"

// Prints the results of a game
void showResults(State *state);

// Prompts human for valid move and does it, returns 1 if the game has finished
int promptHuman(State *state, char *color);

void runHumanVsHuman(void);

// For the moment, numSimulations is a hard count, but later time will be factored in ^^^
void runHumanVsComputer(int numSimulations);

void runComputerVsComputer(int numSimulations);

// Used for testing.  Returns 1 if the computer won, 0 if draw, -1 if lost
int runComputerVsRandom(int numSimulations);

void testComputer(int iterations, int numSimulations);

void runTrial(int numSimulations);

void timeTrials(int trials, int numSimulations);
#endif