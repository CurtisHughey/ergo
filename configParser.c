#include "configParser.h"

Config *getDefaultConfig(void) {
	Config *config = calloc(1, sizeof(config));  // 1 so 0 setting everything
	
	config->rollouts = 1000;
	config->threads = 1;
	config->tests = 25;  // The random vs cpu tests
	config->trials = 5;  // The time trials

	return config;
}

Config *parseConfigFile(char *configFileName) {
	const char commentChar = '#';
	const char* whitespace = " \t";  // Not allowing newlines.  const pointer?

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
	// Huge long ugly if chain.  Definitely could automatically generate
	if (!strcmp(variableName, "rollouts")) {
		if (value < 1) {
			ERROR_PRINT("Number of rollouts must be greater than 0, got: %d", value);
			return 1;
		}
		config->rollouts = value;
		return 0;
	}
	else if (!strcmp(variableName, "numThreads")) {
		if (value < 1) {
			ERROR_PRINT("Number of threads must be greater than 0, got: %d", value);
			return 1;
		}
		config->threads = value;
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