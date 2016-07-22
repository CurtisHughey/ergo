#include "configParser.h"

// Automatically generate ^^^

Config *getDefaultConfig(void) {
	Config *config = calloc(1, sizeof(Config));
	
	config->komiTimes10 = 75;  // Default to 75 (7.5) is a good guess
 	config->rollouts = 30000;
	config->threads = 1;
	config->testGames = 25;  // The random vs cpu tests
	config->trials = 5;  // The time trials
	config->warmupTrials = 2;  // To warm up the cpu for the time trials
	config->lengthOfGame = 350;  // Eh, no idea
	config->unitRandomMakeUnmakeTests = 1000;  // Wow, this is a long name

	return config;
}

Config *parseConfigFile(char *configFileName) {
	const char commentChar = '#';
	const char* whitespace = " \t";

	FILE *fp = fopen(configFileName, "r");

	if (fp == NULL) {
		ERROR_PRINT("Couldn't find file: %s", configFileName);
		exit(1);
	}

	Config *config = getDefaultConfig();

	char line[MAX_LINE];

	while (fgets(line, MAX_LINE, fp) != NULL) {
		char *variable = strtok(line, whitespace);
		if (variable == NULL || variable[0] == commentChar) {  // Either just whitespace or a comment
			continue;  // Disregard
		} else {
			int value = atoi(strtok(NULL, whitespace));
			updateConfig(config, variable, value);
		}
	}

	fclose(fp);

	return config;
}

int updateConfig(Config *config, char *variableName, int value) {
	// Huge long ugly if else chain.  Definitely could automatically generate
	if (!strcmp(variableName, "komiTimes10")) {
		// No bounds on komi (allowed to be negative).  Well, I guess you could exceed int bounds, but why?
		config->komiTimes10 = value;
		return 0;
	} else if (!strcmp(variableName, "rollouts")) {
		if (value < 1) {
			ERROR_PRINT("Number of rollouts must be greater than 0, got: %d", value);
			return 1;
		}
		config->rollouts = value;
		return 0;
	} else if (!strcmp(variableName, "threads")) {
		if (value < 1) {
			ERROR_PRINT("Number of threads must be greater than 0, got: %d", value);
			return 1;
		}
		config->threads = value;
		return 0;
	} else if (!strcmp(variableName, "testGames")) {
		if (value < 1) {
			ERROR_PRINT("Number of tests must be greater than 0, got: %d", value);
			return 1;
		}
		config->testGames = value;
		return 0;
	} else if (!strcmp(variableName, "trials")) {
		if (value < 1) {
			ERROR_PRINT("Number of trials must be greater than 0, got: %d", value);
			return 1;
		}
		config->trials = value;
		return 0;
	} else if (!strcmp(variableName, "warmupTrials")) {
		if (value < 0) {
			ERROR_PRINT("Number of warmup trials must be greater than or equal to 0, got: %d", value);
			return 1;
		}
		config->warmupTrials = value;
		return 0;
	} else if (!strcmp(variableName, "lengthOfGame")) {
		if (value < 1) {
			ERROR_PRINT("Average length of game must be greater than 0, got: %d", value);
			return 1;
		}
		config->lengthOfGame = value;
		return 0;
	} else if (!strcmp(variableName, "unitRandomMakeUnmakeTests")) {
		if (value < 1) {
			ERROR_PRINT("Number of random makeUnmakeTests must be greater than 0, got: %d", value);
			return 1;
		}
		config->unitRandomMakeUnmakeTests = value;
		return 0;
	} else {
		ERROR_PRINT("Unknown variable name: %s", variableName);
		return 1;
	}
}

void destroyConfig(Config *config) {
	free(config);
	config = NULL;
}