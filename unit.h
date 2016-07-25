#ifndef _UNIT_H
#define _UNIT_H

#include <stdio.h>
#include <dirent.h>
#include <time.h>

#include "dbg.h"
#include "state.h"
#include "gameParser.h"
#include "configParser.h"
#include "linkedList.h"

#define NUM_STATE_TESTS 9
#define NUM_LINKEDLIST_TESTS 2


#define RANDOMGAMEITERATIONS 10

typedef struct {
	int errorCode;  // 1 if error, 0 if not
	int totalPasses;
	int totalTests;
} TestResult;

// Entry point to run all unit functions.  Return 0 if full success
int runAllUnitTests(Config *config);

// Returns 0 if full success
// Takes in the name of the test suite, the array of test functions, and the number of them
int runTests(char *testName, TestResult (**tests)(void), int numTests);

TestResult runStateFillWithTests(void);

TestResult runStateGroupBordersTypeAndResetTests(void);

TestResult runStateMakeMoveTests(void);

TestResult runStateMakeUnmakeTests(void);

// Also tests isLegalMoves
TestResult runStateGetMovesTests(void);

TestResult runStateIsLegalMoveTests(void);

TestResult runStateCalcScoresTests(void);

TestResult runStateSetTerritoryTests(void);

// This makes a bunch of random moves and then unmakes them, checks to make sure that the states are equal
TestResult runStateRandomMakeUnmakeTests(void);

////
// List tests
TestResult runLinkedListAdd(void);

TestResult runLinkedListContains(void);

TestResult runLinkedListDelete(void);

TestResult runLinkedListLength(void);

#endif