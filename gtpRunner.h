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

#define LOG_FILE_NAME "log/gtpLogging.txt"

extern const char *commands[];  // Defined in gameRunner.c, function runGtp

#define numCommands (sizeof(commands) / sizeof(const char *))

// Runs GTP to connect with KGS
void runGtp(int rollouts, int lengthOfGame);

// Returns INVALID_MOVE if invalid.  Parses according to GTP specification
int parseGtpMove(char *vertex);

// Converts an internal integer move into a GTP string vertex, or pass.  Must free string
char *moveToGtpString(int move);

// Returns STATE_WHITE or STATE_BLACK given the string, according to GTP.  Returns 0 if error
int stringColorToInt(char *colorString);

#endif
