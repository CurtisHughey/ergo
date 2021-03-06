#include "gameRunner.h"

// At this stage, not really used much
int promptHuman(State *state, char *color, HashTable *hashTable) {
	int gameFinishes = 0;
	do {
		printf("%s to move: ", color);
		int move = parseMoveFromTerminal();
		if (move == MOVE_RESIGN) {
			return MOVE_RESIGN;
		}
		if (isLegalMove(state, move, hashTable)) {
			gameFinishes = state->blackPassed && move == MOVE_PASS; // i.e. both sides are passing, game finishes
			makeMove(state, move, hashTable);
			break;
		} else {
			printf("Illegal move!\n");
		}
	}
	while (1);

	return gameFinishes;
}

void showResults(State *state) {
	printf("Game finished!\n");
	printf("Final position:\n");
	displayState(state);
	Score score = calcScores(state);
	printf("Black: %.1lf\n", score.blackScore);
	printf("White: %.1lf\n", score.whiteScore);

	if (score.blackScore > score.whiteScore) {
		printf("Black won!\n");
	} else if (score.blackScore < score.whiteScore) {
		printf("White won!\n");
	} else /* score.blackScore == score.whiteScore */ {
		printf("Draw!\n");
	}
}

// Not really used much now.  config is unused
void runHumanVsHuman(Config *config) {
	State *state = createState(config->komiTimes10);
	HashTable *hashTable = createHashTable(config->hashBuckets);

	char *colors[2] = { "Black", "White" };

	int status = 0;
	while (!status) {
		for (int i = 0; i < 2; i++) {
			displayState(state);
			status = promptHuman(state, colors[i], hashTable);
			if (status == MOVE_RESIGN) {
				break;
			}
		}
	}

	showResults(state);

	destroyState(state);
	state = NULL;
}

// Not really used much now
void runHumanVsComputer(Config *config) {
	srand(time(NULL));

	State *state = createState(config->komiTimes10);
	HashTable *hashTable = createHashTable(config->hashBuckets);

	char *colors[2] = { "Black", "White" };

	int compTurn = rand() % 2;

	int status = 0;
	while (!status) {
		for (int i = 0; i < 2; i++) {
			displayState(state);
			if (i == compTurn) {  // Could also put this body into function
				int move = uctSearch(state, config, hashTable); 
				char *moveString = moveToString(move, state->turn);
				printf("%s chooses move: %s\n", colors[i], moveString);
				free(moveString);
				moveString = NULL;

				// Checks if resignation, otherwise it makes the move
				if (move == MOVE_RESIGN) {
					status = 1;  // This is kind of wrong, technically we also need to record the loss ^^^
					break;
				}	
				status = state->blackPassed && move == MOVE_PASS;
				makeMove(state, move, hashTable);
			} else {
				status = promptHuman(state, colors[i], hashTable);
				if (status == MOVE_RESIGN) {
					break;
				}
			}
		}
	}

	showResults(state);

	destroyHashTable(hashTable);
	hashTable = NULL;
	destroyState(state);
	state = NULL;
}

void runComputerVsComputer(Config *config) {
	srand(time(NULL)); 

	State *state = createState(config->komiTimes10);
	HashTable *hashTable = createHashTable(config->hashBuckets);

	char *colors[2] = { "Black", "White" };

	int status = 0;
	while (!status) {
		for (int i = 0; i < 2; i++) {
			displayState(state);
			int move = uctSearch(state, config, hashTable); 
			char *moveString = moveToString(move, state->turn);
			printf("%s chooses move: %s\n", colors[i], moveString);
			free(moveString);
			moveString = NULL;	

			// Checks if resignation, otherwise it makes the move
			if (move == MOVE_RESIGN) {
				status = 1;
				break;
			}		
			status = state->blackPassed && move == MOVE_PASS;
			makeMove(state, move, hashTable);
		}
	}

	showResults(state);

	destroyHashTable(hashTable);
	hashTable = NULL;
	destroyState(state);
	state = NULL;
}

// Rand must already be initialized
int runComputerVsRandom(Config *config) {
	State *state = createState(config->komiTimes10);
	HashTable *hashTable = createHashTable(config->hashBuckets);

	int compTurn = rand() % 2;  // 0 for black, 1 for white

	int status = 0;
	while (!status) {
		for (int i = 0; i < 2; i++) {
			int move;
			if (i == compTurn) {  // AI move
				move = uctSearch(state, config, hashTable); 
				// Checks if resignation, otherwise it makes the move
				if (move == MOVE_RESIGN) {
					status = 1;
					break;
				}	
			} else {  // Random move
				Moves *moves = getMoves(state, hashTable);
				move = moves->array[rand() % moves->count];
				
				destroyMoves(moves);
			}

			status = state->blackPassed && move == MOVE_PASS;
			makeMove(state, move, hashTable);
		}
	}

	int color = compTurn == 0 ? STATE_BLACK : STATE_WHITE;  // Converts
	int result = getResult(state, color);

	destroyHashTable(hashTable);
	hashTable = NULL;
	destroyState(state);
	state = NULL;

	return result;
}

int testComputer(Config *config) {
	srand(time(NULL));	

	int iterations = config->testGames;

	int compWon = 0;
	int otherWon = 0;
	int draws = 0;

	const int interval = 1;
	for (int i = 0; i < iterations; i++) {
		int result = runComputerVsRandom(config);
		switch (result) {
			case 1:
				compWon += 1;
				break;
			case -1:
				otherWon += 1;
				break;
			case 0:
				draws += 1;
				break;
			default:
				ERROR_PRINT("Unknown result");
				break;
		}

		if (i % interval == 0) {
			printf(".");
			fflush(stdout);
		}
	}
	printf("\n");

	// Should determine if statistically significant ^^^
	printf("Number of iterations: %d\n", iterations);
	printf("Computer won: %d\n", compWon);
	printf("Other won:    %d\n", otherWon);
	printf("Draws:        %d\n", draws);

	double totalScore = compWon + draws*0.5;

	return totalScore >= 0.8*iterations;  // Means it won most games (especially against 3x3, could lose even with perfect play)
}

// Finds a single move, measures how long
void runTrial(Config *config) {	
	State *state = createState(config->komiTimes10);
	HashTable *hashTable = createHashTable(config->hashBuckets);  // Potentially a lot of startup cost.  Worth using a hashTable?

	int move = uctSearch(state, config, hashTable); 
	makeMove(state, move, hashTable);

	destroyHashTable(hashTable);
	hashTable = NULL;
	destroyState(state);
	state = NULL;
}

void timeTrials(Config *config) {
	srand(time(NULL));

	int warmupTrials = config->warmupTrials;
	int trials = config->trials;

	for (int i = 0; i < warmupTrials; i++) {  // Just to warm up the CPU
		runTrial(config);
	}

	double totalTime = 0;

	Timer timer;
	for (int i = 0; i < trials; i++) {
		startTimer(&timer);
		runTrial(config);
		stopTimer(&timer);

		double elapsedTime = timeElapsed(&timer);

		DEBUG_PRINT("%lf\n", elapsedTime);

		totalTime += elapsedTime;
		printf(".");
		fflush(stdout);
	}
	printf("\n");

	double average = totalTime/trials;
	printf("Average runtime: %lf millis\n", average);

	
	FILE *fp = fopen(PERFORMANCE_FILE, "w");
	if (fp == NULL) {
		ERROR_PRINT("Error opening file, exiting");
		exit(1);
	}

	fprintf(fp, "%d\n", (int)average);
	fclose(fp);
}
