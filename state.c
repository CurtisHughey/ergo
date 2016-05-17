#include "state.h"

State *createState() {
	State *state = malloc(sizeof(State));
	//memset(state->board, 0, sizeof(state->board)); 
	state->turn = STATE_BLACK;
	state->koPoint = -1;  // Nowhere to begin with

	return state;
}

int destroyState(State *state) {
	free(state);
	state = NULL;
	return 0;
}

void displayState(State *state) {
	printf("-----------------------------------------------\n");
	for (int i = BOARD_DIM-1; i >= 0; i--) {
		printf("\t|");
		for (int j = 0; j < BOARD_DIM; j++) {
			char item = 0;
			switch (state->board[i*BOARD_DIM+j]) {
				case STATE_WHITE:
					item = 'W';
					break;
				case STATE_BLACK:
					item = 'B';
					break;
				case STATE_EMPTY:
					item = ' ';
					break;
				case STATE_TRAVERSED:
					item = '!';  // This is bad, probably
					break;
				default:
					item = '?';
					break;
			}
			printf("%c|", item);
		}
		printf("\n");
	}
	printf("\t---------------------------------------\n");
	printf("Turn: %s\n", state->turn == STATE_WHITE ? "White" : "Black");
	printf("Ko point: %d\n", state->koPoint);
	printf("-----------------------------------------------\n");
	printf("\n\n");
}

int isLegalMove(State *state, int move) {

	// Occupied board:
	if (state->board[move] != STATE_EMPTY && state->board[move] != STATE_TRAVERSED) {  // Not sure when the second would come into play
		return 0;
	}

	// Not a ko move:
	if (state->koPoint == move) {
		return 0;
	}

	// Not suicidal:
	state->board[move] = state->turn;  // Makes the move
	if (!hasLiberties(state, move)) {
		// Now needs to find if it kills any of the opponents stones (i.e. there exists a neighboring family that dies)
		int otherTurn = -1*state->turn;

		// Tests each neighbor, resets if done
		Neighbors neighbors = getNeighborsOfType(state, move, otherTurn);

		int found = 0;
		for (int i = 0; i < neighbors.count; i++) {
			if (!hasLiberties(state, neighbors.array[i])) {
				found = 1;
				break;
			}
		}
		if (!found) {
			state->board[move] = STATE_EMPTY;
			return 0;
		}
	}
	// If we're out, reset and return true
	state->board[move] = STATE_EMPTY;

	return 1;
}

// Can still optimize a bit ^^
Neighbors getNeighborsOfType(State *state, int point, int type) {
	Neighbors neighbors;

	int count = 0;

	int col = point % BOARD_DIM;
	int row = point / BOARD_DIM;  // Lol, might be better to do huge switch statement?

	int allMatch = type == STATE_ALL;

	if (col != 0) {
		int position = point-1;
		if (allMatch || state->board[position] == type) {
			neighbors.array[count++] = position;			
		}
	} 
	if (col != BOARD_DIM-1) {
		int position = point+1;
		if (allMatch || state->board[position] == type) {
			neighbors.array[count++] = position;			
		}			
	}
	if (row != 0) {
		int position = point-BOARD_DIM;
		if (allMatch || state->board[position] == type) {
			neighbors.array[count++] = position;			
		}
	} 
	if (row != BOARD_DIM-1) {
		int position = point+BOARD_DIM;
		if (allMatch || state->board[position] == type) {
			neighbors.array[count++] = position;			
		}			
	}	

	neighbors.count = count;

	return neighbors;
}

int hasLiberties(State *state, int point) {
	int stone = state->board[point];
	if (stone == 0) {
		return 0;
	}

	Neighbors neighbors = getNeighborsOfType(state, point, -2);

	for (int i = 0; i < neighbors.count; i++) {
		if (state->board[neighbors.array[i]] == STATE_EMPTY) {  // Found a liberty
			return 1;
		}
		if (neighbors.array[i] == stone) {  // Then keep going  on this one
			state->board[neighbors.array[i]] = STATE_TRAVERSED;  // This represents stones that have already been looked at, ugh
			if (hasLiberties(state, neighbors.array[i])) {
				return 1;
			}
			state->board[neighbors.array[i]] = stone;  // Resets
		}
	}

	return 0;
}

/*State *makeMove(State *state, int move) {
	State *state oldState
}*/