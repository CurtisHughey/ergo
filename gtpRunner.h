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
#include "configParser.h"

#define GTP_MAX_LENGTH 1024  // Conservative, accounting for list_commands, so might eventually need to be extended
#define MAX_GTP_MOVE_LENGTH  10  // Conservative
#define GTP_PROTOCOL_VERSION 2
#define GTP_NAME "Ergo"
#define GTP_VERSION "1.0"

#define LOG_FILE_NAME "log/gtpLogging.txt"

extern const char *commands[];  // Defined in gameRunner.c, function runGtp

#define numCommands (sizeof(commands) / sizeof(const char *))

// Runs GTP to connect with KGS.  Returns 1 if computer wins, -1 if loses, 0 if draw.  -2 if quit
int runGtp(Config *config);

// Returns INVALID_MOVE if invalid.  Parses according to GTP specification
int parseGtpMove(char *vertex);

// Converts an internal integer move into a GTP string vertex, or pass.  Must free string
char *moveToGtpString(int move);

// Returns STATE_WHITE or STATE_BLACK given the string, according to GTP.  Returns 0 if error
int stringColorToInt(char *colorString);

#endif
