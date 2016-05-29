#ifndef _UNIT_H
#define _UNIT_H

#include <stdio.h>
#include <dirent.h>

#include "dbg.h"
#include "state.h"
#include "parser.h"

// Entry point to run all unit functions.  Return 1 if full success
int runAllUnitTests(void);

int runStateTests(void);

int runStateMakeMoveTests(void);

#endif