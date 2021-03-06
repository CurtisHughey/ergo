#include "configParser.h"

// Automatically generate ^^^

Config *getDefaultConfig(void) {
	Config *config = calloc(1, sizeof(Config));
	
	config->komiTimes10 = DEFAULT_KOMITIMES10;  // Defined in state.h
 	config->rollouts = 20000;  // Maybe also make this a function of the sizes of the board
	config->threads = 1;  // Number of threads to do simulations, by default the program runs in serial
	config->testGames = 25;  // The random vs cpu tests
	config->trials = 5;  // The time trials
	config->warmupTrials = 3;  // To warm up the cpu for the time trials
	config->lengthOfGame = (int)(BOARD_SIZE * 1.1);  // This is a big problem, I don't really know ^^^
	config->superko = 1;  // By default, we don't allow superko
	config->hashBuckets = 1000;  // Meh, maybe.  Definitely a tradeoff of memory/speed
	config->respect = -1;  // By default, will never resign (otherwise, give number 0-100, with higher meaning less likely to resign (see bestChild in uct.h))
	config->raveV = 0;  // RAVE constant, 0 means no rave, positive means rave

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
		if (value % 10 != 0 && value % 5 != 0) {
			ERROR_PRINT("10*komi must end in a 5 or a 0, got: %d", value);
			return 1;
		}		
		config->komiTimes10 = value;
	} else if (!strcmp(variableName, "rollouts")) {
		if (value < 1) {
			ERROR_PRINT("Number of rollouts must be greater than 0, got: %d", value);
			return 1;
		}
		config->rollouts = value;
	} else if (!strcmp(variableName, "threads")) {
		if (value < 1) {
			ERROR_PRINT("Number of threads must be greater than 0, got: %d", value);
			return 1;
		}
		config->threads = value;
	} else if (!strcmp(variableName, "testGames")) {
		if (value < 1) {
			ERROR_PRINT("Number of tests must be greater than 0, got: %d", value);
			return 1;
		}
		config->testGames = value;
	} else if (!strcmp(variableName, "trials")) {
		if (value < 1) {
			ERROR_PRINT("Number of trials must be greater than 0, got: %d", value);
			return 1;
		}
		config->trials = value;
	} else if (!strcmp(variableName, "warmupTrials")) {
		if (value < 0) {
			ERROR_PRINT("Number of warmup trials must be greater than or equal to 0, got: %d", value);
			return 1;
		}
		config->warmupTrials = value;
	} else if (!strcmp(variableName, "lengthOfGame")) {
		if (value < 1) {
			ERROR_PRINT("Average length of game must be greater than 0, got: %d", value);
			return 1;
		}
		config->lengthOfGame = value;
	} else if (!strcmp(variableName, "superko")) {
		if (value != 1 && value != 0) {
			ERROR_PRINT("Superko must be 1 to not allow it, otherwise 0, got: %d", value);
			return 1;
		}
		config->superko = value;
	} else if (!strcmp(variableName, "hashBuckets")) {
		if (value < 1) {
			ERROR_PRINT("Number of hash buckets must be greater than 0, got: %d", value);
			return 1;
		}
		config->hashBuckets = value;
	} else if (!strcmp(variableName, "respect")) {
		if (value > 100) {
			ERROR_PRINT("Respect must be less than or equal to 100, got: %d", value);
			return 1;
		}
		config->respect = value;
	} else if (!strcmp(variableName, "raveV")) {
		if (value < 0) {
			ERROR_PRINT("RAVE constant must be non-negative, got: %d", value);
			return 1;
		}
		config->raveV = value;
	} else {
		ERROR_PRINT("Unknown variable name: %s", variableName);
		return 1;
	}
	return 0;  // Successfully parsed
}

void destroyConfig(Config *config) {
	free(config);
	config = NULL;
}
