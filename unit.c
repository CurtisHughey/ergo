#include "unit.h"

int runAllUnitTests(void) {
	return runStateTests();
}

int runStateTests(void) {

	// makeMove
	//runStateMakeMoveTests();

	// makeMove/unmakeMove
	runStateMakeUnmakeTests();  //^^^  not working

	runStateGroupBordersType();

	//////////////

	return 1;
}

int runStateGroupBordersType(void) {
	char filePath[] = "./test/state/groupBordersType/"; 

	DIR *d;
	struct dirent *dir;
	d = opendir(filePath);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_name[0] == 'i') {  
				char *initialFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(initialFile, filePath, strlen(filePath));
				strncpy(initialFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);

				char *expectedFile = calloc(strlen(dir->d_name)+strlen(filePath)+1, sizeof(char));
				strncpy(expectedFile, filePath, strlen(filePath));
				strncpy(expectedFile+strlen(filePath), dir->d_name, strlen(dir->d_name)+1);
				expectedFile[strlen(filePath)] = 'e';  // Makes it the expected one


				State* initialState = parseState(initialFile);

				// Now parses the group details
				// line 1: point
				// line 2: type
				// line 3: yes/no

				FILE *fp = fopen(expectedFile, "r");
				if (fp == NULL) {
					ERROR_PRINT("Couldn't find file: %s", expectedFile);
					exit(1);
				}

				char line[10];  // Way extra space than we need
				fscanf(fp, "%s", line);
				int point = atoi(line);
				fscanf(fp, "%s", line);
				int type = atoi(line);				
				fscanf(fp, "%s", line);
				int expected = atoi(line);	

				if (groupBordersType(initialState, point, type) != expected) {
					printf("NOT EQUAL!!!!\n");
					printf("FOR:");
					displayState(initialState);
				} else {
					printf("EQUAL!!!!\n");
				}
				free(initialFile);
				free(expectedFile);
				destroyState(initialState);
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for groupBordersType");;
		return 0;
	}
	free(d);

	return 1;
}

int runStateMakeMoveTests(void) {
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
				int move = parseMove(moveFile);
				State* expectedState = parseState(expectedFile);

				makeMove(initialState, move);

				if (!statesAreEqual(initialState, expectedState)) {
					printf("NOT EQUAL!!!!\n");
					printf("GOT:");
					displayState(initialState);
				} else {
					printf("EQUAL!!!!\n");
				}
				free(initialFile);
				free(moveFile);
				free(expectedFile);
				destroyState(initialState);
				destroyState(expectedState);
			}
		}
	} else {
		ERROR_PRINT("Couldn't find directory for makeMove");;
		return 0;
	}
	free(d);

	return 1;
}

int runStateMakeUnmakeTests(void) {
	const int iterations = 1000;  // The number of iterations we run through
	const int depth = 2;  // The depth of each iteration

	srand(time(NULL));

	int fails = 0;

	for (int i = 0; i < iterations; i++) {
		State *state = createState();

		for (int j = 0; j < depth; j++) {
			State *copy = copyState(state);
			Moves *moves = getMoves(copy);
			int randIndex = rand() % moves->count;
			UnmakeMoveInfo unmakeMoveInfo;
			if (moves->array[randIndex] > 360) {
				printf(">>>>>%d\n",randIndex);
				printf("count : %d\n", moves->count);
				//exit(1);
			}
			makeMoveAndSave(copy, moves->array[randIndex], &unmakeMoveInfo);
			unmakeMove(copy, &unmakeMoveInfo);

			if (!statesAreEqual(state, copy)) {
				printf("Not equal!!!\n");
				printf("GOT:\n");
				displayState(copy);
				printf("EXPCTED\n");
				displayState(state);
				printf("MOVE: %d\n", moves->array[randIndex]);
				printf("Saved: %d\n", unmakeMoveInfo.move);
				destroyState(copy);
				fails++;
				free(moves);  // Eh, should make function
				break;
			} else {
				makeMove(state, moves->array[randIndex]);  // Now makes move for realsies
			}
			destroyState(copy);
			free(moves);  // Eh, should make function
		}
		destroyState(state);
	}

	printf("FAILS: %d\n",fails);

	return 1;
}
