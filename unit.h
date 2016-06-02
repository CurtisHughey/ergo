#ifndef _UNIT_H
#define _UNIT_H

#include <stdio.h>
#include <dirent.h>
#include <time.h>

#include "dbg.h"
#include "state.h"
#include "parser.h"

// Entry point to run all unit functions.  Return 1 if full success
int runAllUnitTests(void);

int runStateTests(void);

int runStateGroupBordersTypeAndReset(void);

int runStateMakeMoveTests(void);

// This makes a bunch of random moves and then unmakes them, checks to make sure that the states are equal
int runStateMakeUnmakeTests(void);

#endif