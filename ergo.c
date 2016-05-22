#include "ergo.h"

int main(int argc, char **argv) {
	printf("Hello World!\n");

	State *state = createState();
	//state->board[200] = STATE_BLACK;
	state->board[201] = STATE_WHITE;
	state->board[199] = STATE_WHITE;
	state->board[181] = STATE_WHITE;
	state->board[219] = STATE_WHITE;
	state->board[198] = STATE_BLACK;
	state->board[180] = STATE_BLACK;
	state->board[218] = STATE_BLACK;
	displayState(state);

	printf("Is legal: %d\n", isLegalMove(state, 200));

	makeMove(state, 200);
	displayState(state);

	printf("Finding score\n");

	// Moves *moves = getMoves(state);
	// for (int i = 0; i < moves->count; i++) {
	// 	printf("%d, ", moves->array[i]);
	// }
	// printf("\n");

	// serializeState(state, "output/output.txt");
	// destroyState(state);

	// printf("READ STATE:\n");
	// State *readState = parseState("output/output.txt");
	// displayState(readState);

	printf("SCORE: %d\n", calcScore(state, STATE_BLACK));
	displayState(state);


	destroyState(state);
}