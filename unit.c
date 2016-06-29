#include "unit.h"

// Eventually, abstract a lot of the test harness stuff out

int runAllUnitTests(void) {
	printf("------------------------------\n");
	printf("All Tests: \n");
	
	int result = 1;

	result &= runStateTests();

	if (result) {
		printf("ALL PASSED :)\n");
	} else {
		printf("FAILED :(\n");
	}

	printf("------------------------------\n");

	return 1;
}

// Maybe this should just be called runStateTests, TBH
int runStateTests(void) {
	printf("\t--------------------\n");
	printf("\tstate Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	// Add new tests here, also update header file
	TestResult (*stateTests[NUM_TESTS])(void) = { 
									&runStateMakeMoveTests,
									&runStateMakeUnmakeTests,
									&runStateGroupBordersTypeAndResetTests,
									&runFillWithTests,
									&runGetMovesTests,
									&runIsLegalMoveTests,
									&runCalcScoresTests,
									&runSetTerritoryTests,
								};

	for (int i = 0; i < NUM_TESTS; i++) {
		printf("\t\t----------\n");

		TestResult result = stateTests[i]();

		if (result.errorCode) {
			printf("\t\tError >:(");
			continue;  // Any tests that actually happened are ignored
		}

		printf("\t\t%d/%d\n", result.totalPasses, result.totalTests);
		if (result.totalPasses == result.totalTests) {
			printf("\t\tPassed :)\n");
		} else {
			printf("\t\tFailed :(\n");
		}

		totalPasses += result.totalPasses;
		totalTests += result.totalTests;	

		printf("\t\t----------\n");
	}

	//////////////

	int result = 0;
	printf("\t%d/%d\n", totalPasses, totalTests);
	if (totalPasses == totalTests) {
		printf("\tPassed :)\n");
		result = 1;
	} else {
		printf("\tFailed :(\n");
		result = 0;
	}

	printf("\t--------------------\n");

	return result;
}

TestResult runFillWithTests(void) {
	printf("\t\tfillWith Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	char filePath[] = "./test/state/fillWith/"; 

	DIR *d;
	struct dirent *dir;
	d = opendir(filePath);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_name[0] == 'i') {  
				char *initialFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(initialFile, filePath, strlen(filePath));
				strncpy(initialFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);

				char *modFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(modFile, filePath, strlen(filePath));
				strncpy(modFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);
				modFile[strlen(filePath)] = 'm';  // Makes it the move one

				char *expectedFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(expectedFile, filePath, strlen(filePath));
				strncpy(expectedFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);
				expectedFile[strlen(filePath)] = 'e';  // Makes it the expected one

				State* initialState = parseState(initialFile);

				// Now parses the fillWith details
				// line 1: point
				// line 2: type
				FILE *fp = fopen(modFile, "r");
				if (fp == NULL) {
					ERROR_PRINT("Couldn't find file: %s", modFile);
					return (TestResult){1,0,0};
				}

				char line[10];  // Way extra space than we need
				fscanf(fp, "%s", line);
				int point = atoi(line);
				fscanf(fp, "%s", line);
				int type = atoi(line);
				fclose(fp);

				State *expectedState = parseState(expectedFile);

				fillWith(initialState, point, type);

				if (!statesAreEqual(initialState, expectedState)) {
					printf("\t\tFailure for test: %s\n", dir->d_name);
				} else {
					totalPasses += 1;
				}
				totalTests += 1;

				free(initialFile);
				free(modFile);
				free(expectedFile);
				destroyState(initialState);
				destroyState(expectedState);
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for groupBordersTypeAndReset");;
		return (TestResult){1,0,0};
	}
	free(d);

	return (TestResult){0, totalPasses, totalTests};
}	

TestResult runStateGroupBordersTypeAndResetTests(void) {
	printf("\t\tgroupBordersTypeAndReset Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	char filePath[] = "./test/state/groupBordersTypeAndReset/"; 

	DIR *d;
	struct dirent *dir;
	d = opendir(filePath);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_name[0] == 'i') {  
				char *initialFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(initialFile, filePath, strlen(filePath));
				strncpy(initialFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);

				char *modFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(modFile, filePath, strlen(filePath));
				strncpy(modFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);
				modFile[strlen(filePath)] = 'm';  // Makes it the expected one

				State* initialState = parseState(initialFile);

				// Now parses the group details
				// line 1: point
				// line 2: type
				// line 3: yes/no

				FILE *fp = fopen(modFile, "r");
				if (fp == NULL) {
					ERROR_PRINT("Couldn't find file: %s", modFile);
					return (TestResult){1,0,0};
				}

				char line[10];  // Way extra space than we need
				fscanf(fp, "%s", line);
				int point = atoi(line);
				fscanf(fp, "%s", line);
				int type = atoi(line);				
				fscanf(fp, "%s", line);
				int expected = atoi(line);	
				fclose(fp);

				if (groupBordersType(initialState, point, type) != expected) {
					printf("\t\tFailure for test: %s\n", dir->d_name);
				} else {
					totalPasses += 1;
				}
				totalTests += 1;

				free(initialFile);
				free(modFile);
				destroyState(initialState);
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for groupBordersTypeAndReset");;
		return (TestResult){1,0,0};
	}
	free(d);

	return (TestResult){0, totalPasses, totalTests};
}

TestResult runStateMakeMoveTests(void) {
	printf("\t\tmakeMove Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	char filePath[] = "./test/state/makeMove/"; 

	DIR *d;
	struct dirent *dir;
	d = opendir(filePath);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_name[0] == 'i') {  
				char *initialFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(initialFile, filePath, strlen(filePath));
				strncpy(initialFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);

				char *moveFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(moveFile, filePath, strlen(filePath));
				strncpy(moveFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);
				moveFile[strlen(filePath)] = 'm';  // Makes it the move one

				char *expectedFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(expectedFile, filePath, strlen(filePath));
				strncpy(expectedFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);
				expectedFile[strlen(filePath)] = 'e';  // Makes it the expected one

				State* initialState = parseState(initialFile);
				int move = parseMoveFromFile(moveFile);
				State* expectedState = parseState(expectedFile);

				makeMove(initialState, move);

				if (!statesAreEqual(initialState, expectedState)) {
					printf("\t\tFailure for test: %s\n", dir->d_name);
					printf("Got:\n");
					displayState(initialState);
				} else {
					totalPasses += 1;
				}

				totalTests += 1;
				free(initialFile);
				free(moveFile);
				free(expectedFile);
				destroyState(initialState);
				destroyState(expectedState);
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for makeMove");;
		return (TestResult){1,0,0};
	}
	free(d);

	return (TestResult){0, totalPasses, totalTests};
}

TestResult runStateMakeUnmakeTests(void) {
	printf("\t\tmakeUnmake Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	const int iterations = 1000;  // The number of iterations we run through
	const int depth = 250;  // The depth of each iteration (about average for a game)

	srand(time(NULL));

	for (int i = 0; i < iterations; i++) {
		State *state = createState();

		int passed = 1;
		for (int j = 0; j < depth; j++) {
			State *copy = copyState(state);
			Moves *moves = getMoves(copy);
			int randIndex = rand() % moves->count;
			UnmakeMoveInfo unmakeMoveInfo;
			makeMoveAndSave(copy, moves->array[randIndex], &unmakeMoveInfo);
			unmakeMove(copy, &unmakeMoveInfo);

			// Maybe should write this to a file if there's an error, rather than print to terminal ^^^
			if (!statesAreEqual(state, copy)) {
				printf("\t\tFailure\n");
				printf("\t\tGot:\n");
				displayState(copy);
				printf("\t\tExpected\n");
				displayState(state);
				printf("\t\tMove: %d\n", moves->array[randIndex]);
				destroyState(copy);
				free(moves);  // Eh, should make function
				passed = 0;
				break;
			} else {
				makeMove(state, moves->array[randIndex]);  // Now makes move for realsies
			}
			destroyState(copy);
			free(moves);  // Eh, should make function
		}
		if (!passed) {
			printf("\t\tFailure for test: %d\n", i);
		} else {
			totalPasses += 1;
		}
		totalTests += 1;

		destroyState(state);
	}

	return (TestResult){0, totalPasses, totalTests};
}

TestResult runGetMovesTests(void) {
	printf("\t\tgetMoves Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	char filePath[] = "./test/state/getMoves/"; 

	DIR *d;
	struct dirent *dir;
	d = opendir(filePath);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_name[0] == 'i') {  
				char *initialFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(initialFile, filePath, strlen(filePath));
				strncpy(initialFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);

				char *movesFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(movesFile, filePath, strlen(filePath));
				strncpy(movesFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);
				movesFile[strlen(filePath)] = 'm';  // Makes it the move one

				State* initialState = parseState(initialFile);

				FILE *fp = fopen(movesFile, "r");
				if (fp == NULL) {
					ERROR_PRINT("Couldn't find file: %s", movesFile);
					return (TestResult){1,0,0};
				}

				char line[10];  // Way extra space than we need
				fscanf(fp, "%s", line);
				int numMoves = atoi(line);
				fclose(fp);

				Moves *moves = getMoves(initialState);

				int passes = 1;
				if (moves->count != numMoves) {
					printf("\t\tFailure for test: %s\n", dir->d_name);
					printf("Got: %d\n", moves->count);
					passes = 0;
				}

				for (int i = 0; i < moves->count; i++) {
					if (!isLegalMove(initialState, moves->array[i])) {
						printf("\t\tFailure for individual move %d, test: %s\n", moves->array[i], dir->d_name);
						passes = 0;
						break;
					}				
				}

				if (passes) {
					totalPasses += 1;
				}

				totalTests += 1;
				free(moves);
				free(initialFile);
				free(movesFile);
				destroyState(initialState);
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for getMoves");;
		return (TestResult){1,0,0};
	}
	free(d);

	return (TestResult){0, totalPasses, totalTests};
}

TestResult runIsLegalMoveTests(void) {
	printf("\t\tisLegalMoveTests Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	char filePath[] = "./test/state/isLegalMove/"; 

	DIR *d;
	struct dirent *dir;
	d = opendir(filePath);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_name[0] == 'i') {  
				char *initialFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(initialFile, filePath, strlen(filePath));
				strncpy(initialFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);

				char *moveFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(moveFile, filePath, strlen(filePath));
				strncpy(moveFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);
				moveFile[strlen(filePath)] = 'm';  // Makes it the expected one

				State* initialState = parseState(initialFile);

				// Now parses the group details
				// line 1: point
				// line 2: yes/no

				FILE *fp = fopen(moveFile, "r");
				if (fp == NULL) {
					ERROR_PRINT("Couldn't find file: %s", moveFile);
					return (TestResult){1,0,0};
				}

				char line[10];  // Way extra space than we need
				fscanf(fp, "%s", line);
				int move = atoi(line);			
				fscanf(fp, "%s", line);
				int expected = atoi(line);	
				fclose(fp);

				if (isLegalMove(initialState, move) != expected) {
					printf("\t\tFailure for test: %s\n", dir->d_name);
				} else {
					totalPasses += 1;
				}
				totalTests += 1;

				free(initialFile);
				free(moveFile);
				destroyState(initialState);
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for isLegalMove");;
		return (TestResult){1,0,0};
	}
	free(d);

	return (TestResult){0, totalPasses, totalTests};
}

TestResult runCalcScoresTests(void) {
	printf("\t\tcalcScores Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	char filePath[] = "./test/state/calcScores/"; 

	DIR *d;
	struct dirent *dir;
	d = opendir(filePath);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_name[0] == 'i') {  
				char *initialFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(initialFile, filePath, strlen(filePath));
				strncpy(initialFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);

				char *scoreFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(scoreFile, filePath, strlen(filePath));
				strncpy(scoreFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);
				scoreFile[strlen(filePath)] = 'm';  // Makes it the expected one

				State* initialState = parseState(initialFile);

				// Now parses the group details
				// line 1: score for white
				// line 2: score for black

				FILE *fp = fopen(scoreFile, "r");
				if (fp == NULL) {
					ERROR_PRINT("Couldn't find file: %s", scoreFile);
					return (TestResult){1,0,0};
				}

				char line[10];  // Way extra space than we need
				fscanf(fp, "%s", line);
				int whiteScore = atoi(line);
				fscanf(fp, "%s", line);
				int blackScore = atoi(line);		
				fclose(fp);

				Score score = calcScores(initialState);

				if (score.whiteScore != whiteScore || score.blackScore != blackScore) {
					printf("\t\tFailure for test: %s\n", dir->d_name);
					printf("Got scores (%d, %d)\n", score.whiteScore, score.blackScore);
				} else {
					totalPasses += 1;
				}
				totalTests += 1;

				free(initialFile);
				free(scoreFile);
				destroyState(initialState);
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for calcScores");;
		return (TestResult){1,0,0};
	}
	free(d);

	return (TestResult){0, totalPasses, totalTests};
}

TestResult runSetTerritoryTests(void) {
	printf("\t\tTODO setTerritory Tests\n");  // Trying to guilt myself
	return (TestResult){0, 0, 0};
}