#include "unit.h"

// Eventually, abstract a lot of the test harness stuff out
// SOOOOO MUCH COPY AND PASTING

int runAllUnitTests(Config *config) {

	if (BOARD_DIM != 19) {
		ERROR_PRINT("Board must be 19x19!");
		return 1;  // Failure
	}

	printf("------------------------------\n");
	printf("All Tests: \n");
	
	int result = 0;

	TestResult (*stateTests[NUM_STATE_TESTS])(void) = { 
									&runStateGetNeighborsOfType,
									&runStateMakeMoveTests,
									&runStateMakeUnmakeTests,
									&runStateGroupBordersTypeAndResetTests,
									&runStateFillWithTests,
									&runStateGetMovesTests,
									&runStateIsLegalMoveTests,
									&runStateCalcScoresTests,
									&runStateSetTerritoryTests,
									&runStateRandomMakeUnmakeTests,
								};

	TestResult (*listTests[NUM_LINKEDLIST_TESTS])(void) = { 
									&runListAddTests,
									&runListContainsTests,
									&runListDeleteTests,
									&runListLengthTests,
								};

	TestResult (*hashTests[NUM_HASHTABLE_TESTS])(void) = { 
									&runHashAddTests,
									&runHashContainsTests,
									&runHashDeleteTests,
									&runHashSizeTests,
									&runHashGeneralTests,
								};

	result |= runTests("state", stateTests, NUM_STATE_TESTS);  // Using | in anticipation of more tests, 0 means success
	result |= runTests("linkedList", listTests, NUM_LINKEDLIST_TESTS);
	result |= runTests("hash", hashTests, NUM_HASHTABLE_TESTS);

	if (!result) {
		printf("ALL PASSED :)\n");
	} else {
		printf("FAILED :(\n");
	}

	printf("------------------------------\n");

	return result;  // Return 0 if no error, 1 if error
}

int runTests(char *testName, TestResult (**tests)(void), int numTests) {
	printf("\t--------------------\n");
	printf("\t%s Tests: \n", testName);

	int totalPasses = 0;
	int totalTests = 0;

	// Add new tests here, also update header file

	for (int i = 0; i < numTests; i++) {
		printf("\t\t----------\n");

		TestResult result = tests[i]();

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

	int result = 1;
	printf("\t%d/%d\n", totalPasses, totalTests);
	if (totalPasses == totalTests) {
		printf("\tPassed :)\n");
		result = 0;
	} else {
		printf("\tFailed :(\n");
		result = 1;
	}

	printf("\t--------------------\n");

	return result;
}

TestResult runStateGetNeighborsOfType(void) {
	printf("\t\tgetNeighborsOfType Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	char filePath[] = "./test/state/getNeighborsOfType/"; 

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

				State* initialState = parseState(initialFile);

				// Now parses the getNeighborsOfType details
				// line 1: point
				// line 2: type
				// line 3,4,5,6: The points that should get returned in getNeighborsOfType (if fewer, then lines are blank)
				FILE *fp = fopen(modFile, "r");
				if (fp == NULL) {
					ERROR_PRINT("Couldn't find file: %s", modFile);
					return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
				}

				char line[MAX_MOVE_LEN];  // Way extra space than we need
				if (!fgets(line, MAX_MOVE_LEN, fp)) {
					ERROR_PRINT("Failed to parse move");
					exit(1);
				}
				int point = atoi(line);
				if (!fgets(line, MAX_MOVE_LEN, fp)) {
					ERROR_PRINT("Failed to parse move");
					exit(1);
				}
				int type = atoi(line);

				int expectedNeighbors[4];
				int expectedCount = 0;
				while (fgets(line, MAX_MOVE_LEN, fp) && expectedCount < 4) {
					expectedNeighbors[expectedCount++] = atoi(line); 
				} 

				fclose(fp);

				Neighbors actualNeighbors;
				getNeighborsOfType(initialState, point, type, &actualNeighbors);

				// First compares the sizes
				if (actualNeighbors.count != expectedCount) {
					printf("\t\tFailure for test: %s\n", dir->d_name);
					printf("\t\tGot count of %d\n", actualNeighbors.count);
				} else {
					// Now sort arrays and then compare
					qsort(expectedNeighbors, expectedCount, sizeof(int), compareInts);
					qsort(actualNeighbors.array, actualNeighbors.count, sizeof(int), compareInts);
				
					int passed = 1;
					for (int i = 0; i < expectedCount; i++) {
						if (actualNeighbors.array[i] != expectedNeighbors[i]) {
							printf("\t\tFailure for test: %s\n", dir->d_name);
							passed = 0;

							printf("\t\tGot: ");
							for (int j = 0; j < actualNeighbors.count; j++) {
								printf("%d, ", actualNeighbors.array[j]);
							}
							printf("\n");
							break;
						}				
					}
					
					if (passed) {
						totalPasses += 1;
					}
				}

				totalTests += 1;

				free(initialFile);
				free(modFile);

				destroyState(initialState);
				initialState = NULL;
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for getNeighborsOfType");;
		return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
	}
	closedir(d);

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

TestResult runStateFillWithTests(void) {
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
					return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
				}

				char line[MAX_MOVE_LEN];  // Way extra space than we need
				if (!fgets(line, MAX_MOVE_LEN, fp)) {
					ERROR_PRINT("Failed to parse move");
					exit(1);
				}
				int point = atoi(line);
				if (!fgets(line, MAX_MOVE_LEN, fp)) {
					ERROR_PRINT("Failed to parse move");
					exit(1);
				}
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
				initialState = NULL;
				destroyState(expectedState);
				expectedState = NULL;
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for fillWith");;
		return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
	}
	closedir(d);

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
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
					return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
				}

				char line[MAX_MOVE_LEN];  // Way extra space than we need
				if (!fgets(line, MAX_MOVE_LEN, fp)) {
					ERROR_PRINT("Failed to parse move");
					exit(1);
				}
				int point = atoi(line);
				if (!fgets(line, MAX_MOVE_LEN, fp)) {  // Ok, technically not a move, whatever
					ERROR_PRINT("Failed to parse move");
					exit(1);
				}
				int type = atoi(line);				
				if (!fgets(line, MAX_MOVE_LEN, fp)) {
					ERROR_PRINT("Failed to parse move");
					exit(1);
				}
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
				initialState = NULL;
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for groupBordersTypeAndReset");;
		return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
	}
	closedir(d);

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
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

				makeMove(initialState, move, NULL);

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
				initialState = NULL;
				destroyState(expectedState);
				expectedState = NULL;
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for makeMove");;
		return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
	}
	closedir(d);

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

TestResult runStateMakeUnmakeTests(void) {
	printf("\t\tmakeUnmakeMove Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	char filePath[] = "./test/state/makeUnmakeMove/"; 

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

				State* initialState = parseState(initialFile);
				int move = parseMoveFromFile(moveFile);

				State *copy = copyState(initialState);
				UnmakeMoveInfo unmakeMoveInfo;
				makeMoveAndSave(initialState, move, &unmakeMoveInfo, NULL);
				unmakeMove(initialState, &unmakeMoveInfo, NULL);

				if (!statesAreEqual(initialState, copy)) {
					printf("\t\tFailure for test: %s\n", dir->d_name);
					printf("Got:\n");
					displayState(initialState);
				} else {
					totalPasses += 1;
				}

				totalTests += 1;
				free(initialFile);
				free(moveFile);

				destroyState(initialState);
				initialState = NULL;
				destroyState(copy);
				copy = NULL;
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for makeUnmakeMove");;
		return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
	}
	closedir(d);

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

TestResult runStateRandomMakeUnmakeTests(void) {
	printf("\t\trandomMakeUnmake Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	const int depth = 250;  // The depth of each iteration (about average for a game)

	srand(time(NULL));

	for (int i = 0; i < RANDOMGAMEITERATIONS; i++) {
		State *state = createState(DEFAULT_KOMITIMES10);

		int passed = 1;
		for (int j = 0; j < depth; j++) {
			State *copy = copyState(state);
			Moves *moves = getMoves(copy, NULL);
			int randIndex = rand() % moves->count;
			UnmakeMoveInfo unmakeMoveInfo;
			makeMoveAndSave(copy, moves->array[randIndex], &unmakeMoveInfo, NULL);
			unmakeMove(copy, &unmakeMoveInfo, NULL);

			// Maybe should write this to a file if there's an error, rather than print to terminal ^^^
			if (!statesAreEqual(state, copy)) {
				printf("\t\tFailure\n");
				printf("\t\tGot:\n");
				displayState(copy);
				printf("\t\tExpected\n");
				displayState(state);
				printf("\t\tMove: %d\n", moves->array[randIndex]);

				destroyState(copy);
				copy = NULL;
				destroyMoves(moves);
				moves = NULL;
				passed = 0;
				break;
			} else {
				makeMove(state, moves->array[randIndex], NULL);  // Now makes move for realsies
			}

			destroyState(copy);
			copy = NULL;
			destroyMoves(moves);
			moves = NULL;
		}
		if (!passed) {
			printf("\t\tFailure for test: %d\n", i);
		} else {
			totalPasses += 1;
		}
		totalTests += 1;

		destroyState(state);
		state = NULL;
	}

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

TestResult runStateGetMovesTests(void) {
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
					return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
				}

				char line[MAX_MOVE_LEN];  // Way extra space than we need
				if (!fgets(line, MAX_MOVE_LEN, fp)) {
					ERROR_PRINT("Failed to parse move");
					exit(1);
				}
				int numMoves = atoi(line);
				fclose(fp);

				Moves *moves = getMoves(initialState, NULL);

				int passes = 1;
				if (moves->count != numMoves) {
					printf("\t\tFailure for test: %s\n", dir->d_name);
					printf("Got: %d\n", moves->count);
					passes = 0;
				}

				for (int i = 0; i < moves->count; i++) {
					if (!isLegalMove(initialState, moves->array[i], NULL)) {
						printf("\t\tFailure for individual move %d, test: %s\n", moves->array[i], dir->d_name);
						passes = 0;
						break;
					}				
				}

				if (passes) {
					totalPasses += 1;
				}

				totalTests += 1;

				destroyMoves(moves);
				moves = NULL;

				free(initialFile);
				free(movesFile);

				destroyState(initialState);
				initialState = NULL;
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for getMoves");;
		return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
	}
	closedir(d);

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

TestResult runStateIsLegalMoveTests(void) {
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
					return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
				}

				char line[MAX_MOVE_LEN];  // Way extra space than we need
				if (!fgets(line, MAX_MOVE_LEN, fp)) {
					ERROR_PRINT("Failed to parse move");
					exit(1);
				}
				int move = atoi(line);			
				if (!fgets(line, MAX_MOVE_LEN, fp)) {
					ERROR_PRINT("Failed to parse move");
					exit(1);
				}
				int expected = atoi(line);	
				fclose(fp);

				if (isLegalMove(initialState, move, NULL) != expected) {
					printf("\t\tFailure for test: %s\n", dir->d_name);
				} else {
					totalPasses += 1;
				}
				totalTests += 1;

				free(initialFile);
				free(moveFile);

				destroyState(initialState);
				initialState = NULL;
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for isLegalMove");;
		return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
	}
	closedir(d);

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

TestResult runStateCalcScoresTests(void) {
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
				// line 1: score for black
				// line 2: score for white

				FILE *fp = fopen(scoreFile, "r");
				if (fp == NULL) {
					ERROR_PRINT("Couldn't find file: %s", scoreFile);
					return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
				}

				char line[MAX_MOVE_LEN];  // Way extra space than we need
				if (!fgets(line, MAX_MOVE_LEN, fp)) {
					ERROR_PRINT("Failed to parse move");
					exit(1);
				}
				double blackScore = atof(line);		
				if (!fgets(line, MAX_MOVE_LEN, fp)) {
					ERROR_PRINT("Failed to parse move");
					exit(1);
				}
				double whiteScore = atof(line);
				fclose(fp);

				Score score = calcScores(initialState);

				if (score.whiteScore != whiteScore || score.blackScore != blackScore) {
					printf("\t\tFailure for test: %s\n", dir->d_name);
					printf("Got scores (%lf, %lf)\n", score.whiteScore, score.blackScore);
				} else {
					totalPasses += 1;
				}
				totalTests += 1;

				free(initialFile);
				free(scoreFile);

				destroyState(initialState);
				initialState = NULL;
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for calcScores");;
		return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
	}
	closedir(d);

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

TestResult runStateSetTerritoryTests(void) {
	printf("\t\tTODO setTerritory Tests\n");  // Trying to guilt myself.  Ugh, still haven't done it, I suck
	return (TestResult){ .errorCode = 0, .totalPasses = 0, .totalTests = 0 };
}

// List and hash table tests.  Note that the list tests don't take in files, we just hardcode
TestResult runListAddTests(void) {

	printf("\t\tlistAdd Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	const int numTests = 2;

	for (int i = 1; i <= numTests; i++) {
		totalTests += 1;

		int passed = 0;  // Whether the test passed or not

		Node *head = NULL;  // Sets up the list
		switch (i) {  // Decides which test to do (this way, we can easily identify the failed tests)
			case 1: 
				listAdd(&head, 2);

				passed = head->hashValue == 2;
				break;

			case 2: 
				listAdd(&head, 2);
				listAdd(&head, 1);

				passed = head->hashValue == 1 && head->next->hashValue == 2;
				break;

			case 3: 
				listAdd(&head, 2);

				passed = !listAdd(&head, 2);  // Makes sure there is an error if trying to add again
				break;

			default:
				ERROR_PRINT("Set the wrong number of tests");
				passed = 0;  // Might as well
				break;
		}

		if (!passed) {
			printf("\t\tFailure for test: %d\n", i);
			printf("\t\tGot: ");
			listPrint(&head);
		} else {
			totalPasses += 1;
		}

		listClear(&head);  // Tears down the list
	}

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

TestResult runListContainsTests(void) {

	printf("\t\tlistContains Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	const int numTests = 5;

	for (int i = 1; i <= numTests; i++) {
		totalTests += 1;

		int passed = 0;  // Whether the test passed or not

		Node *head = NULL;  // Sets up the list
		switch (i) {  // Decides which test to do (this way, we can easily identify the failed tests)
			case 1: 
				listAdd(&head, 2);

				passed = listContains(&head, 2);
				break;

			case 2: 
				listAdd(&head, 2);
				listAdd(&head, 3);

				passed = listContains(&head, 2) && listContains(&head, 3);
				break;

			case 3: 
				passed = !listContains(&head, 2);
				break;

			case 4: 
				listAdd(&head, 2);

				passed = !listContains(&head, 3);
				break;

			case 5:
				passed = !listContains(NULL, 3);  // Making sure NULL is handled correctly

				break;

			default:
				ERROR_PRINT("Set the wrong number of tests");
				passed = 0;  // Might as well
				break;
		}

		if (!passed) {
			printf("\t\tFailure for test: %d\n", i);
			printf("\t\tGot: ");
			listPrint(&head);
		} else {
			totalPasses += 1;
		}

		listClear(&head);  // Tears down the list
	}

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

TestResult runListDeleteTests(void) {

	printf("\t\tlistDelete Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	const int numTests = 5;

	for (int i = 1; i <= numTests; i++) {
		totalTests += 1;

		int passed = 0;  // Whether the test passed or not

		Node *head = NULL;  // Sets up the list
		switch (i) {  // Decides which test to do (this way, we can easily identify the failed tests)
			case 1: 
				listAdd(&head, 2);
				listDelete(&head, 2);

				passed = !listContains(&head, 2);
				break;

			case 2: 
				listAdd(&head, 2);
				listAdd(&head, 3);
				listDelete(&head, 2);

				passed = !listContains(&head, 2) && listContains(&head, 3);
				break;

			case 3: 
				listAdd(&head, 2);
				listAdd(&head, 3);
				listDelete(&head, 3);

				passed = !listContains(&head, 3) && listContains(&head, 2);
				break;

			case 4: 
				listAdd(&head, 2);

				passed = listDelete(&head, 3);
				break;

			case 5: 
				passed = listDelete(&head, 3);
				break;

			default:
				ERROR_PRINT("Set the wrong number of tests");
				passed = 0;  // Might as well
				break;
		}

		if (!passed) {
			printf("\t\tFailure for test: %d\n", i);
			printf("\t\tGot: ");
			listPrint(&head);
		} else {
			totalPasses += 1;
		}

		listClear(&head);  // Tears down the list
	}

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

// This is a good catch-all test to see false positives
TestResult runListLengthTests(void) {

	printf("\t\tlistLength Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	const int numTests = 8;

	for (int i = 1; i <= numTests; i++) {
		totalTests += 1;

		int passed = 0;  // Whether the test passed or not

		Node *head = NULL;  // Sets up the list
		switch (i) {  // Decides which test to do (this way, we can easily identify the failed tests)
			case 1: 
				passed = listLength(&head) == 0;
				break;

			case 2: 
				listAdd(&head, 2);

				passed = listLength(&head) == 1;
				break;

			case 3: 
				listAdd(&head, 2);
				listAdd(&head, 3);

				passed = listLength(&head) == 2;
				break;

			case 4:
				listAdd(&head, 2);
				listAdd(&head, 2);

				passed = listLength(&head) == 2;  // We currently allow duplicates
				break;

			case 5:
				listAdd(&head, 2);
				listAdd(&head, 3);
				listDelete(&head, 3);

				passed = listLength(&head) == 1;
				break;

			case 6:
				listAdd(&head, 2);
				listAdd(&head, 3);
				listDelete(&head, 3);
				listDelete(&head, 2);

				passed = listLength(&head) == 0;  // Second add ignored
				break;

			case 7:
				listAdd(&head, 2);
				listAdd(&head, 3);
				listDelete(&head, 2);  // Inverting order of deleting from last case
				listDelete(&head, 3);

				passed = listLength(&head) == 0;  // Second add ignored
				break;

			case 8:
				listAdd(&head, 2);
				listAdd(&head, 3);
				listDelete(&head, 3);  // Inverting order of deleting from last case
				listAdd(&head, 3);

				passed = listLength(&head) == 2;  // Second add ignored
				break;

			default:
				ERROR_PRINT("Set the wrong number of tests");
				passed = 0;  // Might as well
				break;
		}

		if (!passed) {
			printf("\t\tFailure for test: %d\n", i);
			printf("\t\tGot: ");
			listPrint(&head);
		} else {
			totalPasses += 1;
		}

		listClear(&head);  // Tears down the list
	}

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

// Nothing much we can easily do to verify, just seeing if errors occur
TestResult runHashAddTests(void) {
	printf("\t\taddToHashTable Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	const int numTests = 3;

	State *s1 = createState(DEFAULT_KOMITIMES10);
	State *s2 = createState(DEFAULT_KOMITIMES10);
	s2->board[0] = STATE_WHITE;  // Just to make it different

	for (int i = 1; i <= numTests; i++) {
		totalTests += 1;

		int passed = 0;  // Whether the test passed or not

		HashTable *hashTable = createHashTable(500); // Sets up the hash table
		switch (i) {  // Decides which test to do (this way, we can easily identify the failed tests)
			case 1: 
				addToHashTable(hashTable, s1);

				passed = 1;
				break;

			case 2: 
				addToHashTable(hashTable, s1);
				addToHashTable(hashTable, s2);

				passed = 1;
				break;

			case 3: 
				addToHashTable(hashTable, s2);
				addToHashTable(hashTable, s2);  // Seeing what happens when we add to same bucket
				
				passed = 1;
				break;

			default:
				ERROR_PRINT("Set the wrong number of tests");
				passed = 0;  // Might as well
				break;
		}

		if (!passed) {
			printf("\t\tFailure for test: %d\n", i);  // Unfortunately, not easy to display
		} else {
			totalPasses += 1;
		}

		destroyHashTable(hashTable);  // Tears it down
		hashTable = NULL;
	}

	destroyState(s1);
	s1 = NULL;
	destroyState(s2);
	s2 = NULL;

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

TestResult runHashContainsTests(void) {
	printf("\t\tcontainsInHashTable Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	const int numTests = 4;

	State *s1 = createState(DEFAULT_KOMITIMES10);
	State *s2 = createState(DEFAULT_KOMITIMES10);
	State *s3 = createState(DEFAULT_KOMITIMES10);
	State *s4 = createState(DEFAULT_KOMITIMES10);
	State *s5 = createState(DEFAULT_KOMITIMES10);
	State *s6 = createState(DEFAULT_KOMITIMES10);
	s2->board[0] = STATE_WHITE;
	s3->board[0] = STATE_BLACK;
	s4->koPoint = 0;
	s5->board[1] = STATE_WHITE;

	for (int i = 1; i <= numTests; i++) {
		totalTests += 1;

		int passed = 0;  // Whether the test passed or not

		HashTable *hashTable = createHashTable(500); // Sets up the hash table, 500 buckets, who cares
		switch (i) {  // Decides which test to do (this way, we can easily identify the failed tests)
			case 1: 
				addToHashTable(hashTable, s1);

				passed = containsInHashTable(hashTable, s1);
				break;

			case 2: 
				addToHashTable(hashTable, s1);
				addToHashTable(hashTable, s2);

				passed = containsInHashTable(hashTable, s1) && containsInHashTable(hashTable, s2);
				break;

			case 3: 
				addToHashTable(hashTable, s1);
				
				passed = containsInHashTable(hashTable, s1) && !containsInHashTable(hashTable, s2);
				break;

			case 4: 				
				passed = !containsInHashTable(hashTable, s1);
				break;

			case 5:
				addToHashTable(hashTable, s1);

				passed = !containsInHashTable(hashTable, s2);
				break;

			case 6:
				addToHashTable(hashTable, s1);

				passed = !containsInHashTable(hashTable, s3);
				break;

			case 7:
				addToHashTable(hashTable, s2);

				passed = !containsInHashTable(hashTable, s3);
				break;

			case 8:
				addToHashTable(hashTable, s1);

				passed = !containsInHashTable(hashTable, s4);
				break;

			case 9:
				addToHashTable(hashTable, s1);

				passed = !containsInHashTable(hashTable, s5);
				break;

			case 10:
				addToHashTable(hashTable, s1);

				passed = containsInHashTable(hashTable, s6);  // Should hash to same value
				break;

			default:
				ERROR_PRINT("Set the wrong number of tests");
				passed = 0;  // Might as well
				break;
		}

		if (!passed) {
			printf("\t\tFailure for test: %d\n", i);
		} else {
			totalPasses += 1;
		}

		destroyHashTable(hashTable);  // Tears it down
		hashTable = NULL;
	}

	destroyState(s1);
	s1 = NULL;
	destroyState(s2);
	s2 = NULL;
	destroyState(s3);
	s3 = NULL;
	destroyState(s4);
	s4 = NULL;
	destroyState(s5);
	s5 = NULL;
	destroyState(s6);
	s6 = NULL;

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

TestResult runHashDeleteTests(void) {
	printf("\t\tdeleteFromHashTable Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	const int numTests = 4;

	State *s1 = createState(DEFAULT_KOMITIMES10);
	State *s2 = createState(DEFAULT_KOMITIMES10);
	s2->board[0] = STATE_WHITE;

	for (int i = 1; i <= numTests; i++) {
		totalTests += 1;

		int passed = 0;  // Whether the test passed or not

		HashTable *hashTable = createHashTable(500); // Sets up the hash table
		switch (i) {  // Decides which test to do (this way, we can easily identify the failed tests)
			case 1: 
				addToHashTable(hashTable, s1);
				deleteFromHashTable(hashTable, s1);

				passed = !containsInHashTable(hashTable, s1);
				break;

			case 2: 
				addToHashTable(hashTable, s1);
				addToHashTable(hashTable, s2);
				deleteFromHashTable(hashTable, s1);

				passed = !containsInHashTable(hashTable, s1) && containsInHashTable(hashTable, s2);
				break;

			case 3: 
				addToHashTable(hashTable, s1);
				
				passed = deleteFromHashTable(hashTable, s2);  // Deleting nothing, should return 1
				break;

			case 4: 
				addToHashTable(hashTable, s1);
				addToHashTable(hashTable, s1);
				deleteFromHashTable(hashTable, s1);

				passed = containsInHashTable(hashTable, s1);  // Copies are allowed for now
				break;

			default:
				ERROR_PRINT("Set the wrong number of tests");
				passed = 0;  // Might as well
				break;
		}

		if (!passed) {
			printf("\t\tFailure for test: %d\n", i);
		} else {
			totalPasses += 1;
		}

		destroyHashTable(hashTable);  // Tears it down
		hashTable = NULL;
	}

	destroyState(s1);
	s1 = NULL;
	destroyState(s2);
	s2 = NULL;

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

TestResult runHashSizeTests(void) {
	printf("\t\tsizeOfHashTable Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	const int numTests = 5;

	State *s1 = createState(DEFAULT_KOMITIMES10);
	State *s2 = createState(DEFAULT_KOMITIMES10);
	s2->board[0] = STATE_WHITE;

	for (int i = 1; i <= numTests; i++) {
		totalTests += 1;

		int passed = 0;  // Whether the test passed or not

		HashTable *hashTable = createHashTable(500); // Sets up the hash table
		switch (i) {  // Decides which test to do (this way, we can easily identify the failed tests)
			case 1: 
				addToHashTable(hashTable, s1);

				passed = sizeOfHashTable(hashTable) == 1;
				break;

			case 2: 
				passed = sizeOfHashTable(hashTable) == 0;
				break;

			case 3: 
				addToHashTable(hashTable, s1);
				deleteFromHashTable(hashTable, s1);

				passed = sizeOfHashTable(hashTable) == 0;
				break;

			case 4: 
				addToHashTable(hashTable, s1);
				addToHashTable(hashTable, s2);
				deleteFromHashTable(hashTable, s1);

				passed = sizeOfHashTable(hashTable) == 1;
				break;

			case 5: 
				addToHashTable(hashTable, s1);
				addToHashTable(hashTable, s1);

				passed = sizeOfHashTable(hashTable) == 2;
				break;

			default:
				ERROR_PRINT("Set the wrong number of tests");
				passed = 0;  // Might as well
				break;
		}

		if (!passed) {
			printf("\t\tFailure for test: %d\n", i);
		} else {
			totalPasses += 1;
		}

		destroyHashTable(hashTable);  // Tears it down
		hashTable = NULL;
	}

	destroyState(s1);
	s1 = NULL;
	destroyState(s2);
	s2 = NULL;

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

TestResult runHashGeneralTests(void) {
	printf("\t\thashGeneral Tests: \n");

	int totalPasses = 0;
	int totalTests = 0;

	char filePath[] = "./test/hash/hashGeneral/"; 

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
				HashTable *hashTable = createHashTable(1);  // Arbitrary bucket number, doesn't really matter ish
				addToHashTable(hashTable, initialState);  // By default the initial state is not added, so we need to do it ourselves

				FILE *fp = fopen(movesFile, "r");
				if (fp == NULL) {
					ERROR_PRINT("Couldn't find file: %s", movesFile);
					return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
				}

				char line[MAX_MOVE_LEN];  // Way extra space than we need
				if (!fgets(line, MAX_MOVE_LEN, fp)) {
					ERROR_PRINT("Failed to parse move");
					exit(1);
				}
				int numMoves = atoi(line);  // The number of moves to be executed, the last of which should be illegal by superko
				
				int lastMove = MOVE_INVALID;
				int moveCounter = 0;
				while (fgets(line, MAX_MOVE_LEN, fp)) {
					lastMove = parseMove(line);
					moveCounter += 1;
					if (moveCounter < numMoves) {  // Then should make the move
						makeMove(initialState, lastMove, hashTable);
					}
				} 
				fclose(fp);

				if (isLegalMove(initialState, lastMove, hashTable)) {  // Makes sure the last move is illegal
					printf("\t\tFailure for test: %s\n", dir->d_name);
				} else {
					totalPasses += 1;
				}

				totalTests += 1;

				free(initialFile);
				free(movesFile);

				destroyHashTable(hashTable);
				hashTable = NULL;
				destroyState(initialState);
				initialState = NULL;
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for hashGeneral");;
		return (TestResult){ .errorCode = 1, .totalPasses = 0, .totalTests = 0 };
	}
	closedir(d);

	return (TestResult){ .errorCode = 0, .totalPasses = totalPasses, .totalTests = totalTests };
}

int compareInts(const void *a, const void *b) {
	const int *aVal = (const int *)a;
	const int *bVal = (const int *)b;

	return (*aVal)-(*bVal);
}
