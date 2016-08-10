#include "state.h"

// State struct is in stateInfo.h, along with a lot of macros (needed to resolve some circular dependencies)

static double komi_g = 0;  // Global so we don't have to pass it freaking everywhere.  Sucks to do a global variable, though :/

void setKomi(double komi) {
	komi_g = komi;
}

State *createState() {
	State *state = malloc(sizeof(State));  // Check for success
	clearBoard(state);

	return state;
}

void clearBoard(State *state) {  // This functions is called by createState, also in GTP protocol
	memset(state->board, STATE_EMPTY, sizeof(state->board));  // Empties it.  This requires that STATE_EMPTY is 0, it's kind of bad
	
	state->turn = STATE_BLACK;
	state->koPoint = -1;  // Nowhere to begin with
	state->whitePrisoners = 0;
	state->blackPrisoners = 0;
	state->blackPassed = 0;
}

int destroyState(State *state) {
	free(state);
	state = NULL;
	
	return 0;
}

// This function is a little kooky - it will not copy the hash table, only set to NULL.  If you need it, you must set it on your own
State *copyState(State *original) {
	State *copy = malloc(sizeof(State));

	memcpy(copy->board, original->board, sizeof(original->board));
	copy->turn = original->turn;
	copy->koPoint = original->koPoint;
	copy->whitePrisoners = original->whitePrisoners;
	copy->blackPrisoners = original->blackPrisoners;
	copy->blackPassed = original->blackPassed;
	
	return copy;
}

void displayState(State *state) {
	printf("-----------------------------------------------\n");
	for (int i = BOARD_DIM-1; i >= 0; i--) {
		printf("\t%c |", 'a'+i);
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
	printf("\t   ");
	for (int i = 0; i < BOARD_DIM; i++) {
		printf("%c ", 'a'+i);
	}
	printf("\n");
	printf("\t---------------------------------------\n");
	printf("Turn: %s\n", state->turn == STATE_WHITE ? "White" : "Black");
	printf("White stones captured: %d\n",state->whitePrisoners);
	printf("Black stones captured: %d\n",state->blackPrisoners);
	printf("Ko point: %d\n", state->koPoint);
	printf("Black passed: %d\n", state->blackPassed);
	printf("-----------------------------------------------\n");
	printf("\n\n");
}

// Should specify how it's illegal
int isLegalMove(State *state, int move, HashTable *hashTable) {
	if (move == MOVE_PASS) {
		return 1;  // Always legal
	}

	// Bound checking
	if (move < 0 || move >= BOARD_SIZE) {  // Could be < -1 if parsed as MOVE_INVALID, or MOVE_RESIGN.  MOVE_PASS already covered above, which is also negative
		return 0;  // Duh
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
	if (!groupBordersTypeAndReset(state, move, STATE_EMPTY)) {  // Checks to see if it has any liberties (if it does, then legal)
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

	// If we're out, reset
	state->board[move] = STATE_EMPTY;

	// Superko:
	if (hashTable != NULL) {  // Then we're tracking superko (MOVE_PASS already taken care of)
		UnmakeMoveInfo unmakeMoveInfo;  // Unfortunate that I have to go through all this
		makeMoveAndSave(state, move, &unmakeMoveInfo, NULL);  // Pass in NULL to the hash table because we don't want to add this value
		
		int legal = !containsInHashTable(hashTable, state);
		unmakeMove(state, &unmakeMoveInfo, NULL);

		if (!legal) {  // Then the position would repeat a previous position
			return 0;
		}
	}

	return 1;
}

void getNeighborsOfType(State *state, int point, int type, Neighbors *neighbors) {
	int count = 0;

	if (point != MOVE_PASS) {  // I guess it's good to be defensive here?
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
			return 1;
		}
		if (state->board[allNeighbors.array[i]] == stone) {  // Then keep going on this one
			if (groupBordersType(state, allNeighbors.array[i], type)) {
				return 1;
			}
		}		
	}
	return 0;
}

int groupBordersTypeAndReset(State *state, int point, int type) {
	int savedType = state->board[point];
	int result = groupBordersType(state, point, type);
	fillWith(state, point, savedType);
	return result;
}

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

// Assumes legal move!
void makeMove(State *state, int move, HashTable *hashTable) {
	if (move == MOVE_RESIGN || move == MOVE_INVALID) {  // Might as well check
		ERROR_PRINT("Move: %d\n passed to makeMove", move);
		return;  // Do nothing
	}

	if (move != MOVE_PASS) {  // Not passing (otherwise, don't do anything)

		// Now make the move
		state->board[move] = state->turn;

		// Now maybe erase enemies
		int otherTurn = OTHER_COLOR(state->turn);

		int totalCaptured = 0;
		int capturePoint = -1;  // Used for ko

		Neighbors allNeighbors; 
		getNeighborsOfType(state, move, STATE_ALL, &allNeighbors);
		int enemyCount = 0;
		for (int i = 0; i < allNeighbors.count; i++) {
			if (state->board[allNeighbors.array[i]] == otherTurn) {  // Could've been set to empty by previous iterations
				if (!groupBordersTypeAndReset(state, allNeighbors.array[i], STATE_EMPTY)) {
					totalCaptured += fillWith(state, allNeighbors.array[i], STATE_EMPTY);
					capturePoint = allNeighbors.array[i];
				}
				enemyCount += 1;
			}
		}

		if (state->turn == STATE_WHITE) {
			state->blackPrisoners += totalCaptured;
		} else {  /* state->turn == STATE_BLACK */
			state->whitePrisoners += totalCaptured;
		}

		// Now maybe sets ko
		// This means we put down our stone, was surrounded, and one stone was removed (then ko comes into play)
		if (totalCaptured == 1 && allNeighbors.count == enemyCount) {
			state->koPoint = capturePoint;  // removePoint was only set once
		} else {
			state->koPoint = -1;  // Resetting
		}

		state->blackPassed = 0;
	} else if (state->turn == STATE_BLACK) {  // This is to help record the end of the game
		state->blackPassed = 1;  // Make this one liner
	} else {
		state->blackPassed = 0;
	}

	// Now sets turn (also needs to happen for pass)
	state->turn = OTHER_COLOR(state->turn);

	if (move != MOVE_PASS && hashTable != NULL) {  // If it's equal to MOVE_PASS, then the position had already been stored.  hashTable == NULL means we aren't tracking
		int result = addToHashTable(hashTable, state);  // The initial position is never stored, probs doesn't matter ^^^
		if (result) {
			ERROR_PRINT("Failed to add");
		}
	}

	return;
}

void makeMoveAndSave(State *state, int move, UnmakeMoveInfo *unmakeMoveInfo, HashTable *hashTable) {
	unmakeMoveInfo->move = move;
	unmakeMoveInfo->koPoint = state->koPoint;

	int otherTurn = OTHER_COLOR(state->turn);	
	Neighbors enemyNeighbors;
	getNeighborsOfType(state, move, otherTurn, &enemyNeighbors);

	unmakeMoveInfo->blackPassed = state->blackPassed;

	makeMove(state, move, hashTable);

	int count = 0;
	for (int i = 0; i < enemyNeighbors.count; i++) {
		if (state->board[enemyNeighbors.array[i]] == STATE_EMPTY) {  // Then it was just captured
			unmakeMoveInfo->needToFill.array[count++] = enemyNeighbors.array[i];
		}
	}
	unmakeMoveInfo->needToFill.count = count;

	return;
}

void unmakeMove(State *state, UnmakeMoveInfo *unmakeMoveInfo, HashTable *hashTable) {
	if (unmakeMoveInfo->move != MOVE_PASS) {
		if (hashTable != NULL) {
			int result = deleteFromHashTable(hashTable, state);  // Delete the current state from the hash table (only if not MOVE_PASS, because then the positions are allowed to repeat)
			if (result) {
				ERROR_PRINT("Failed to delete");
			}		
		}

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
	state->blackPassed = unmakeMoveInfo->blackPassed;

	return;
}

// We created this function in case we wanna make the array on the heap instead of the stack
Moves *createMoves(void) {
	Moves *moves = malloc(sizeof(Moves));
	moves->size = BOARD_SIZE/2;  // Kind of magic, guess for the number of moves.  May have to realloc
	moves->array = calloc(moves->size, sizeof(int));
	moves->count = 0;  // Nothing added yet

	return moves;	
}

// Should optimize, right now it's O(n^2) calling isLegalMove every time could be rough
// Should keep track of how many liberties each stone is directly and indirectly connected to. edit: (hmm, maybe)
Moves *getMoves(State *state, HashTable *hashTable) {
	Moves *moves = createMoves();

	for (int i = 0; i < BOARD_SIZE; i++) {
		if (isLegalMove(state, i, hashTable)) {  // This is another linear op
			moves->array[moves->count++] = i;
			
			if (moves->count == moves->size-1) {  // Then need to realloc.  -1 to allow the MOVE_PASS to always be successfully added
				moves->size *= 2;  // Doubles it
				moves->array = realloc(moves->array, moves->size*sizeof(int));
			}
		}
	}

	moves->array[moves->count++] = MOVE_PASS;  // Passing, always legal

	return moves;
}

void destroyMoves(Moves *moves) {
	free(moves->array);
	moves->array = NULL;

	free(moves);
	moves = NULL;
}

int calcScore(State *state, int type) {
	// First have to do a slightly annoying check to see if the entire board is empty
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
	double whiteScore = (double)calcScore(state, STATE_WHITE)+komi_g;  // komi_g is a global variable defined at the top of the file
	double blackScore = (double)calcScore(state, STATE_BLACK);

	return (Score){ .whiteScore = whiteScore, .blackScore = blackScore};
}

int getResult(State *state, int color) {
	Score score = calcScores(state);

	if (score.blackScore == score.whiteScore) {
		return 0;  // draw
	} else {
		// This could get refactored better
		int result = -1;
		if (color == STATE_BLACK) {
			if (score.blackScore > score.whiteScore) {
				result = 1;
			}
		} else {  /* color == STATE_WHITE */
			if (score.blackScore < score.whiteScore) {
				result = 1;
			}			
		}
		return result;
	}	
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

	if (state1->blackPassed != state2->blackPassed) {
		return 0;
	}

	return 1;
}