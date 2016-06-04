#include "state.h"

State *createState(void) {
	State *state = malloc(sizeof(State));
	memset(state->board, STATE_EMPTY, sizeof(state->board)); 
	state->turn = STATE_BLACK;
	state->koPoint = -1;  // Nowhere to begin with
	state->whitePrisoners = 0;
	state->blackPrisoners = 0;

	return state;
}

int destroyState(State *state) {
	free(state);
	state = NULL;
	return 0;
}

State *copyState(State *original) {
	State *copy = malloc(sizeof(State));
	memcpy(copy->board, original->board, sizeof(original->board));
	copy->turn = original->turn;
	copy->koPoint = original->koPoint;
	copy->whitePrisoners = original->whitePrisoners;
	copy->blackPrisoners = original->blackPrisoners;

	return copy;
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
				case STATE_YES:
					item = 'Y';
					break;
				case STATE_NO:
					item = 'N';
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
	printf("White stones captured: %d\n",state->whitePrisoners);
	printf("Black stones captured: %d\n",state->blackPrisoners);
	printf("Ko point: %d\n", state->koPoint);
	printf("-----------------------------------------------\n");
	printf("\n\n");
}

int isLegalMove(State *state, int move) {
	if (move == MOVE_PASS) {
		return 1;
	}

	// Occupied board:
	if (state->board[move] == STATE_WHITE || state->board[move] == STATE_BLACK) {
		return 0;
	}

	// Not a ko move:
	if (state->koPoint == move) {
		return 0;
	}

	// Not suicidal:
	state->board[move] = state->turn;  // Makes the move
	if (!groupBordersTypeAndReset(state, move, STATE_EMPTY)) {
		// Now needs to find if it kills any of the opponents stones (i.e. there exists a neighboring family that dies)
		int otherTurn = OTHER_COLOR(state->turn);

		// Tests each neighbor, resets if done
		Neighbors neighbors;
		getNeighborsOfType(state, move, otherTurn, &neighbors);

		int found = 0;
		for (int i = 0; i < neighbors.count; i++) {
			if (!groupBordersTypeAndReset(state, neighbors.array[i], STATE_EMPTY)) {
				found = 1;
				break;
			}
		}
		if (!found) {  // Then all the enemy stones are living, so it's illegal
			state->board[move] = STATE_EMPTY;
			return 0;
		}
	}

	// If we're out, reset and return true
	state->board[move] = STATE_EMPTY;

	return 1;
}

void getNeighborsOfType(State *state, int point, int type, Neighbors *neighbors) {
	int count = 0;

	if (point != MOVE_PASS) {  // ??? ^^^
		int col = point % BOARD_DIM;
		int row = point / BOARD_DIM; 

		int allMatch = type == STATE_ALL;

		if (col != 0) {
			int position = point-1;
			if (allMatch || state->board[position] == type) {
				neighbors->array[count++] = position;			
			}
		} 
		if (col != BOARD_DIM-1) {
			int position = point+1;
			if (allMatch || state->board[position] == type) {
				neighbors->array[count++] = position;			
			}			
		}
		if (row != 0) {
			int position = point-BOARD_DIM;
			if (allMatch || state->board[position] == type) {
				neighbors->array[count++] = position;			
			}
		} 
		if (row != BOARD_DIM-1) {
			int position = point+BOARD_DIM;
			if (allMatch || state->board[position] == type) {
				neighbors->array[count++] = position;			
			}			
		}	
	}

	neighbors->count = count;

	return;
}

int groupBordersType(State *state, int point, int type) {
	int stone = state->board[point];  // (Not necessarily a stone, could be empty)

	Neighbors allNeighbors;
	getNeighborsOfType(state, point, STATE_ALL, &allNeighbors);

	state->board[point] = STATE_TRAVERSED;

	for (int i = 0; i < allNeighbors.count; i++) {
		if (state->board[allNeighbors.array[i]] == type) {
			//state->board[point] = stone;
			return 1;
		}
		if (state->board[allNeighbors.array[i]] == stone) {  // Then keep going on this one
			if (groupBordersType(state, allNeighbors.array[i], type)) {
				//state->board[point] = stone;
				return 1;
			}
		}		
	}
	//state->board[point] = stone;
	return 0;
}

int groupBordersTypeAndReset(State *state, int point, int type) {
	int savedType = state->board[point];
	int result = groupBordersType(state, point, type);
	fillWith(state, point, savedType);
	return result;
}

// Probably shouldn't make this and groupBordersType recursive ^^
int fillWith(State *state, int point, int type) {
	int stone = state->board[point];
	if (stone == type) {  // Then initial call was wrong
		return 0;
	}

	state->board[point] = type;

	int total = 1;

	// Now recursively looks at matching neighbors
	Neighbors matchingNeighbors;
	getNeighborsOfType(state, point, stone, &matchingNeighbors);
	for (int i = 0; i < matchingNeighbors.count; i++) {
		if (state->board[matchingNeighbors.array[i]] == stone) {  // It could've been changed within the recursion of another iteration
			total += fillWith(state, matchingNeighbors.array[i], type);
		}
	}

	return total;
}

int setTerritory(State *state, int point, int color) {
	if (state->board[point] != STATE_EMPTY) {
		return -1;  // Shouldnt've have been called
	}

	int otherType = OTHER_COLOR(color);

	if (!groupBordersTypeAndReset(state, point, otherType)) {
		fillWith(state, point, STATE_YES);
		return 1;  // Maybe want to return how many filled?
	} else {
		fillWith(state, point, STATE_NO);
		return 0;
	}
}

void makeMove(State *state, int move) {
	if (move != MOVE_PASS) {  // Not passing (otherwise, don't do anything)

		// Now make the move
		state->board[move] = state->turn;

		// Now maybe erase enemies
		int otherTurn = OTHER_COLOR(state->turn);

		int totalCaptured = 0;
		int capturePoint = -1;  // Used for ko

		Neighbors enemyNeighbors; 
		getNeighborsOfType(state, move, otherTurn, &enemyNeighbors);
		for (int i = 0; i < enemyNeighbors.count; i++) {
			if (state->board[enemyNeighbors.array[i]] != STATE_EMPTY) {  // Could've been set to empty by previous iterations
				if (!groupBordersTypeAndReset(state, enemyNeighbors.array[i], STATE_EMPTY)) {
					totalCaptured += fillWith(state, enemyNeighbors.array[i], STATE_EMPTY);
					capturePoint = enemyNeighbors.array[i];
				}
			}
		}

		if (state->turn == STATE_WHITE) {
			state->blackPrisoners += totalCaptured;
		} else {  /* state->turn == STATE_BLACK */
			state->whitePrisoners += totalCaptured;
		}

		// Now maybe sets ko
		// This means we put down our stone, was surrounded, and one stone was removed (then ko comes into play)
		if (totalCaptured == 1 && enemyNeighbors.count == NUM_NEIGHBORS) {
			state->koPoint = capturePoint;  // removePoint was only set once
		} else {
			state->koPoint = -1;  // Resetting
		}
	}

	// Now sets turn (also needs to happen for pass)
	state->turn = OTHER_COLOR(state->turn);

	return;
}

void makeMoveAndSave(State *state, int move, UnmakeMoveInfo *unmakeMoveInfo) {
	unmakeMoveInfo->move = move;
	unmakeMoveInfo->koPoint = state->koPoint;

	int otherTurn = OTHER_COLOR(state->turn);	
	Neighbors enemyNeighbors;
	getNeighborsOfType(state, move, otherTurn, &enemyNeighbors);

	makeMove(state, move);

	int count = 0;
	for (int i = 0; i < enemyNeighbors.count; i++) {
		if (state->board[enemyNeighbors.array[i]] == STATE_EMPTY) {  // Then it was just captured
			unmakeMoveInfo->needToFill.array[count++] = enemyNeighbors.array[i];
		}
	}
	unmakeMoveInfo->needToFill.count = count;

	return;
}

void unmakeMove(State *state, UnmakeMoveInfo *unmakeMoveInfo) {
	if (unmakeMoveInfo->move != MOVE_PASS) {
		// First fills in captured stones
		int total = 0;
		for (int i = 0; i < unmakeMoveInfo->needToFill.count; i++) {
			if (state->board[unmakeMoveInfo->needToFill.array[i]] != state->turn) {  // Might have already been filled in in a previous iteration
				total += fillWith(state, unmakeMoveInfo->needToFill.array[i], state->turn);
			}
		}
		if (state->turn == STATE_BLACK) {
			state->blackPrisoners -= total;
		} else { /* state->type == STATE_WHITE */
			state->whitePrisoners -= total;
		}
		state->board[unmakeMoveInfo->move] = STATE_EMPTY;  // Removes the move
	}

	state->koPoint = unmakeMoveInfo->koPoint;
	state->turn = OTHER_COLOR(state->turn);

	return;
}

// Should optimize ^^^, calling isLegalMove every time could be rough
Moves *getMoves(State *state) {
	Moves *moves = malloc(sizeof(Moves));
	int count = 0;

	for (int i = 0; i < BOARD_SIZE; i++) {
		if (isLegalMove(state, i)) {
			moves->array[count++] = i;
		}
	}

	moves->array[count++] = MOVE_PASS;  // Passing... 

	moves->count = count;

	return moves;
}

int calcScore(State *state, int type) {
	// First have to do a slightly annoying check to see if the entire board is empty
	// Maybe combine with for loop below ^^^
	int empty = 1;
	for (int i = 0; i < BOARD_SIZE; i++) {
		if (state->board[i] != STATE_EMPTY) {
			empty = 0;
			break;
		}
	}
	if (empty) {  // Then the entire board is empty, no one owns anything
		return 0;
	}

	int numStones = 0;

	// First counts score
	for (int i = 0; i < BOARD_SIZE; i++) {
		if (state->board[i] == type) {
			numStones += 1;
		}
	}

	int numEyes = 0;
	// Now finds eyes
	for (int i = 0; i < BOARD_SIZE; i++) {
		switch (state->board[i]) {
			case STATE_EMPTY:
				if (setTerritory(state, i, type)) {
					numEyes += 1;
				}	
				break;
			case STATE_YES:
				numEyes += 1;
				break;
			default:
				break;			
		}
	}

	// Now reset the board
	for (int i = 0; i < BOARD_SIZE; i++) {
		if (state->board[i] == STATE_YES || state->board[i] == STATE_NO) {
			state->board[i] = STATE_EMPTY;
		}
	}

	int totalScore = numStones + numEyes;

	return totalScore;
}

Score calcScores(State *state) {
	int whiteScore = calcScore(state, STATE_WHITE);
	int blackScore = calcScore(state, STATE_BLACK);

	return (Score){whiteScore, blackScore};
}

int statesAreEqual(State *state1, State *state2) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		if (state1->board[i] != state2->board[i]) {
			return 0;
		}
	}

	if (state1->turn != state2->turn) {
		return 0;
	}

	if (state1->koPoint != state2->koPoint) {
		return 0;
	}

	if (state1->whitePrisoners != state2->whitePrisoners) {
		return 0;
	}

	if (state1->blackPrisoners != state2->blackPrisoners) {
		return 0;
	}

	return 1;
}