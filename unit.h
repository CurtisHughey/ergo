#ifndef _UNIT_H
#define _UNIT_H

#include <stdio.h>
#include <dirent.h>
#include <time.h>

#include "dbg.h"
#include "state.h"
#include "gameParser.h"
#include "configParser.h"

#define NUM_TESTS 9

typedef struct {
	int errorCode;  // 1 if error, 0 if not
	int totalPasses;
	int totalTests;
} TestResult;

// Entry point to run all unit functions.  Return 0 if full success
int runAllUnitTests(Config *config);

// Returns 0 if full success
int runStateTests(int stateRandomIterations);

TestResult runFillWithTests(void);

TestResult runStateGroupBordersTypeAndResetTests(void);

TestResult runStateMakeMoveTests(void);

TestResult runStateMakeUnmakeTests(void);

// Also tests isLegalMoves
TestResult runGetMovesTests(void);

TestResult runIsLegalMoveTests(void);

TestResult runCalcScoresTests(void);

TestResult runSetTerritoryTests(void);

// This makes a bunch of random moves and then unmakes them, checks to make sure that the states are equal
TestResult runStateRandomMakeUnmakeTests(void);

#endif