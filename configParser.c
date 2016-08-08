#include "configParser.h"

// Automatically generate ^^^

Config *getDefaultConfig(void) {
	Config *config = calloc(1, sizeof(Config));
	
	config->komiTimes10 = 75;  // Default to 75 (7.5) is a good guess
 	config->rollouts = 100;  // Maybe also make this a function of the sizes of the board
	config->threads = 2;  // Number of threads to do simulations
	config->testGames = 25;  // The random vs cpu tests
	config->trials = 5;  // The time trials
	config->warmupTrials = 3;  // To warm up the cpu for the time trials
	config->lengthOfGame = (int)(BOARD_SIZE * 1.1);  // This is a big problem, I don't really know ^^^
	config->superko = 1;  // By default, we don't allow superko
	config->hashBuckets = 1000;  // Meh, maybe.  Definitely a tradeoff of memory/speed
	config->respect = -1;  // By default, will never resign (otherwise, give number 0-100, with higher meaning less likely to resign (see bestChild in uct.h))

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
	} else if (!strcmp(variableName, "superko")) {
		if (value != 1 && value != 0) {
			ERROR_PRINT("Superko must be 1 to not allow it, otherwise 0, got: %d", value);
			return 1;
		}
		config->superko = value;
		return 0;
	} else if (!strcmp(variableName, "hashBuckets")) {
		if (value < 1) {
			ERROR_PRINT("Number of hash buckets must be greater than 0, got: %d", value);
			return 1;
		}
		config->hashBuckets = value;
		return 0;
	} else if (!strcmp(variableName, "resign")) {
		if (value > 100) {
			ERROR_PRINT("Number of hash buckets must be less than or equal to 100, got: %d", value);
			return 1;
		}
		config->respect = value;
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