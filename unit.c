#include "unit.h"

int runAllUnitTests(void) {
	return runStateTests();
}

int runStateTests(void) {

	// makeMove
	runStateMakeMoveTests();

	//////////////

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
			}
		}
	}

	return 1;
}

