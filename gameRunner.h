#ifndef _GAMERUNNER_H
#define _GAMERUNNER_H

#include <stdio.h>

#include "state.h"
#include "gameParser.h"
#include "uct.h"
#include "timer.h"
#include "dbg.h"

// This was a bit hacky, would be nice to configure ^^^
#define PERFORMANCE_FILE "perf/rawPerformance.txt"

// Prints the results of a game
void showResults(State *state);

// Prompts human for valid move and does it, returns 1 if the game has finished
int promptHuman(State *state, char *color);

void runHumanVsHuman(void);

// For the moment, rollouts is a hard count, but later time will be factored in ^^^
void runHumanVsComputer(int rollouts, int lengthOfGame);

void runComputerVsComputer(int rollouts, int lengthOfGame);

// Used for testing.  Returns 1 if the computer won, 0 if draw, -1 if lost
int runComputerVsRandom(int rollouts, int lengthOfGame);

// Returns 0 if computer passes (wins sufficient number of games)
int testComputer(int iterations, int rollouts, int lengthOfGame);

void runTrial(int rollouts, int lengthOfGame);

// Writes the average time in millis to the default performance file
void timeTrials(int warmupTrials, int trials, int rollouts, int lengthOfGame);

#endif