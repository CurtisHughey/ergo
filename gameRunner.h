#ifndef _GAMERUNNER_H
#define _GAMERUNNER_H

#include <stdio.h>

#include "state.h"
#include "gameParser.h"
#include "uct.h"
#include "timer.h"
#include "dbg.h"
#include "configParser.h"
#include "hash.h"

// This was a bit hacky, would be nice to configure ^^^
#define PERFORMANCE_FILE "perf/rawPerformance.txt"

// Prints the results of a game
void showResults(State *state);

// Prompts human for valid move and does it, returns 1 if the game has finished
int promptHuman(State *state, char *color, HashTable *hashTable);

// Runs human vs human game
void runHumanVsHuman(Config *config);

// For the moment, rollouts is a hard count, but later time will be factored in ^^^
void runHumanVsComputer(Config *config);

// Runs computer vs computer game
void runComputerVsComputer(Config *config);

// Used for testing.  Returns 1 if the computer won, 0 if draw, -1 if lost
int runComputerVsRandom(Config *config);

// Returns 0 if computer passes (wins sufficient number of games)
int testComputer(Config *config);

// Runs a single trial from timeTrials
void runTrial(Config *config);

// Writes the average time in millis to the default performance file
void timeTrials(Config *config);

#endif