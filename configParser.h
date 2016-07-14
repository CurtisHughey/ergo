#ifndef _CONFIGPARSER_H
#define _CONFIGPARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "dbg.h"

#define MAX_LINE 256  // Really conservative
#define MAX_VAR 128

// All options to run the program
typedef struct {
	int rollouts;
	int threads;
	int tests;
	int trials;
	int unitRandomMakeUnmakeTests;
} Config;

// Default configurations
Config *getDefaultConfig(void );

// Parses configuration file, returns Config struct.  If variable not specified, default is used
Config *parseConfigFile(char *configFileName);

// Updates for a single variable in the config.  Returns 0 if successful, 1 if not
int updateConfig(Config *config, char *variableName, int value);

void destroyConfig(Config *config);

#endif