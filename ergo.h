#ifndef _ERGO_H
#define _ERGO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "state.h"
#include "dbg.h"
#include "gameParser.h"
#include "gameRunner.h"
#include "gtpRunner.h"
#include "unit.h"
#include "configParser.h"

// Specifies the functions called in gameRunner.c or gtpRunner.c
typedef enum {
	NONE,
	UNIT,
	HVH,
	HVC,
	CVC,
	CVR,
	TIME,
	GTP,
} functions;


int main(int argc, char **argv);

#endif