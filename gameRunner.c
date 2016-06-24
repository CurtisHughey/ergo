#include "gameRunner.h"

int promptHuman(State *state, char *color) {
	int gameFinishes = 0;
	do {
		printf("%s to move: ", color);
		int move = parseMoveFromTerminal();
		if (move == QUIT) {
			return QUIT;
		}
		if (isLegalMove(state, move)) {
			gameFinishes = state->blackPassed && move == MOVE_PASS; // i.e. both sides are passing, game finishes
			makeMove(state, move);  // eventually makeMoveAndSave
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
	printf("Black: %d\n", score.blackScore);
	printf("White: %d\n", score.whiteScore);

	if (score.blackScore > score.whiteScore) {
		printf("Black won!\n");
	} else if (score.blackScore < score.whiteScore) {
		printf("White won!\n");
	} else /* score.blackScore == score.whiteScore */ {
		printf("Draw!\n");
	}
}

// Add ability to undo move ^^^
void runHumanVsHuman(void) {
	State *state = createState();

	char *colors[2] = { "Black", "White" };

	int status = 0;
	while (!status) {
		for (int i = 0; i < 2; i++) {
			displayState(state);
			status = promptHuman(state, colors[i]);
			if (status == QUIT) {
				break;
			}
		}
	}

	showResults(state);

	destroyState(state);
}

void runHumanVsComputer(void) {
	State *state = createState();

	char *colors[2] = { "Black", "White" };

	srand(time(NULL));
	int compTurn = rand() % 2;  // 0 for black, 1 for white

	int status = 0;
	while (!status) {
		for (int i = 0; i < 2; i++) {
			displayState(state);
			if (i == compTurn) {  // Could also put this body into function
				int move = uctSearch(state, 1000);  // Lol, IDK about 1000 (it's so wrong ^^^)
				printf("%s chooses move: %d\n", colors[i], move);  // Shoud translate to string move ^^^
				status = state->blackPassed && move == MOVE_PASS;
				makeMove(state, move);
			} else {
				status = promptHuman(state, colors[i]);
				if (status == QUIT) {
					break;
				}
			}
		}
	}

	showResults(state);

	destroyState(state);
}

void runComputerVsComputer(void) {	
	State *state = createState();

	char *colors[2] = { "Black", "White" };

	int status = 0;
	while (!status) {
		for (int i = 0; i < 2; i++) {
			displayState(state);
			int move = uctSearch(state, 500); 
			printf("%s chooses move: %d\n", colors[i], move);
			status = state->blackPassed && move == MOVE_PASS;
			makeMove(state, move);
		}
	}

	showResults(state);

	destroyState(state);	
}

int runComputerVsRandom(void) {
	State *state = createState();

	srand(time(NULL));
	int compTurn = rand() % 2;  // 0 for black, 1 for white

	int status = 0;
	while (!status) {
		for (int i = 0; i < 2; i++) {
			int move;
			if (i == compTurn) {  // AI move
				move = uctSearch(state, 100); 
			} else {  // Random move
				Moves *moves = getMoves(state);
				move = moves->array[rand() % moves->count];
			}

			status = state->blackPassed && move == MOVE_PASS;
			makeMove(state, move);
		}
	}

	Score score = calcScores(state);

	if (score.blackScore == score.whiteScore) {
		return 0;  // draw
	} else {
		// This could get refactored better ^^^
		int result = -1;
		if (compTurn == STATE_BLACK) {
			if (score.blackScore > score.whiteScore) {
				result = 1;
			}
		} else {  /* compTurn == STATE_WHITE */
			if (score.blackScore < score.whiteScore) {
				result = 1;
			}			
		}
		return result;
	}
}

void testComputer(int iterations) {
	int compWon = 0;
	int otherWon = 0;
	int draws = 0;

	const int interval = 5;
	for (int i = 0; i < iterations; i++) {
		int result = runComputerVsRandom();
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
		}
	}
	printf("\n");

	// Should determine if statistically significant ^^^
	printf("Number of iterations: %d\n", iterations);
	printf("Computer won: %d\n", compWon);
	printf("Other won:    %d\n", otherWon);
	printf("Draws:        %d\n", draws);
}
