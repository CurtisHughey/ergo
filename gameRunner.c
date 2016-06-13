#include "gameRunner.h"

// Add ability to undo move ^^^
void runHumanVsHuman(void) {
	State *state = createState();

	char *colors[2] = { "Black", "White" };
	UNUSED(colors);

	int gameFinishes = 0;
	while (!gameFinishes) {
		for (int i = 0; i < 2; i++) {
			displayState(state);
			do {
				printf("%s to move: ", colors[i]);
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
		}
	}

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

	destroyState(state);
}