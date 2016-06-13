#include "gameRunner.h"

int promptHuman(State *state, char *color) {
	int gameFinishes = 0;
	do {
		printf("%s to move: ", color);
		int move = parseMoveFromTerminal();
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

	int gameFinishes = 0;
	while (!gameFinishes) {
		for (int i = 0; i < 2; i++) {
			displayState(state);
			gameFinishes = promptHuman(state, colors[i]);
		}
	}

	showResults(state);

	destroyState(state);
}

void runHumanVsComputer(void) {
	State *state = createState();

	srand(time(NULL));
	int compTurn = rand() % 2;  // 0 for black, 1 for white

	char *colors[2] = { "Black", "White" };

	int gameFinishes = 0;
	while (!gameFinishes) {
		for (int i = 0; i < 2; i++) {
			displayState(state);
			if (i == compTurn) {  // Could also put this body into function
				int move = uctSearch(state);
				printf("%s chooses move: %d\n", colors[i], move);  // Shoud translate to string move ^^^
				gameFinishes = state->blackPassed && move == MOVE_PASS;
				makeMove(state, move);
			} else {
				gameFinishes = promptHuman(state, colors[i]);
			}
		}
	}

	showResults(state);

	destroyState(state);
}