#include "ergo.h"

int main(int argc, char **argv) {
	printf("Hello World!\n");

	State *state = createState(19);
	//state->board[200] = STATE_BLACK;
	state->board[201] = STATE_WHITE;
	state->board[199] = STATE_WHITE;
	state->board[181] = STATE_WHITE;
	state->board[219] = STATE_WHITE;
	state->board[198] = STATE_BLACK;
	state->board[180] = STATE_BLACK;
	//state->board[218] = STATE_BLACK;
	displayState(state);

	printf("Result: %d\n", isLegalMove(state, 200));
	destroyState(state);
}