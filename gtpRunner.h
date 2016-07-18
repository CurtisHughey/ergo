#ifndef _GTPRUNNER_H
#define _GTPRUNNER_H

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#include "state.h"
#include "gameParser.h"
#include "uct.h"
#include "dbg.h"

#define GTP_MAX_LENGTH 1024  // Ish? ^^^
#define GTP_PROTOCOL_VERSION 2
#define GTP_NAME "Ergo"
#define GTP_VERSION "1.0"

extern const char *commands[];  // Defined in gameRunner.c, function runGtp

#define numCommands (sizeof(commands) / sizeof(const char *))

// Runs GTP to connect with KGS
void runGtp(int rollouts, int lengthOfGame);

// Returns INVALID_MOVE if invalid.  Parses according to GTP specification
int parseGtpMove(char *vertex);

#endif
