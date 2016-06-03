#ifndef _UNIT_H
#define _UNIT_H

#include <stdio.h>
#include <dirent.h>
#include <time.h>

#include "dbg.h"
#include "state.h"
#include "parser.h"

typedef struct {
	int errorCode;  // 1 if error, 0 if not
	int totalPasses;
	int totalTests;
} TestResult;

// Entry point to run all unit functions.  Return 1 if full success
int runAllUnitTests(void);

int runStateTests(void);

TestResult runFillWithTests(void);

TestResult runStateGroupBordersTypeAndResetTests(void);

TestResult runStateMakeMoveTests(void);

// This makes a bunch of random moves and then unmakes them, checks to make sure that the states are equal
TestResult runStateMakeUnmakeTests(void);

#endif