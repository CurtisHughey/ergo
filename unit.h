#ifndef _UNIT_H
#define _UNIT_H

#include <stdio.h>
#include <dirent.h>
#include <time.h>

#include "dbg.h"
#include "state.h"
#include "hash.h"
#include "gameParser.h"
#include "configParser.h"
#include "linkedList.h"

#define NUM_STATE_TESTS 10
#define NUM_LINKEDLIST_TESTS 4
#define NUM_HASHTABLE_TESTS 4

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

//////////
// State tests
TestResult runStateGetNeighborsOfType(void);

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

//////////
// List tests
TestResult runListAddTests(void);

TestResult runListContainsTests(void);

TestResult runListDeleteTests(void);

TestResult runListLengthTests(void);

//////////
// Hash tests

TestResult runHashAddTests(void);

TestResult runHashContainsTests(void);

TestResult runHashDeleteTests(void);

TestResult runHashSizeTests(void);


//////////
// Utility functions

int compareInts(const void *a, const void *b);

//////////

#endif