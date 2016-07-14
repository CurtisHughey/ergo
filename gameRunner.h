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

// For the moment, rollouts is a hard count, but later time will be factored in ^^^
void runHumanVsComputer(int rollouts);

void runComputerVsComputer(int rollouts);

// Used for testing.  Returns 1 if the computer won, 0 if draw, -1 if lost
int runComputerVsRandom(int rollouts);

// Returns 0 if computer passes (wins sufficient number of games)
int testComputer(int iterations, int rollouts);

void runTrial(int rollouts);

// Returns average time in millis
int timeTrials(int trials, int rollouts);
#endif