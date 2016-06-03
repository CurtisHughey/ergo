#include "parser.h"

// Parses line by line, each point is either W, B, or space (" ")
// Then some extra metadata (turn, ko, etc.)
State *parseState(char *fileName) {
	State *state = createState();

	FILE *fp = fopen(fileName, "r");

	if (fp == NULL) {
		ERROR_PRINT("Couldn't find file: %s", fileName);
		exit(1);
	}

	char line[BOARD_DIM+1];

	for (int i = BOARD_DIM-1; i >= 0; i--) {
		if (fscanf(fp, "%s", line) == EOF) {
			ERROR_PRINT("Not enough lines");
			return NULL;  // Incorrect formatting
		}

		for (int j = 0; j < BOARD_DIM; j++) {
			int point = i*BOARD_DIM+j;
			switch (line[j]) {
				case 'W':
					state->board[point] = STATE_WHITE;
					break;
				case 'B':
					state->board[point] = STATE_BLACK;
					break;
				case '-':
					state->board[point] = STATE_EMPTY;
					break;
				case '\0': 
					ERROR_PRINT("Not enough on a line");
					return NULL;  // Error
				default:
					ERROR_PRINT("Illegal character: %c at %d", line[j], point);
					return NULL;
			}
		}
	}
	// Next line should be turn (either W or B)
	if (fscanf(fp, "%s", line) == EOF) {
		ERROR_PRINT("Missing turn");
		return NULL;  // Incorrect formatting		
	}
	state->turn = line[0] == 'W' ? STATE_WHITE : STATE_BLACK;

	// White stones captured (number)
	if (fscanf(fp, "%s", line) == EOF) {
		ERROR_PRINT("Missing white prisoners");
		return NULL;  // Incorrect formatting		
	}
	state->whitePrisoners = atoi(line);

	// White stones captured (number)
	if (fscanf(fp, "%s", line) == EOF) {
		ERROR_PRINT("Missing black prisoners");
		return NULL;  // Incorrect formatting		
	}
	state->blackPrisoners = atoi(line);

	// Ko point (-1 if none)	
	if (fscanf(fp, "%s", line) == EOF) {
		ERROR_PRINT("Missing ko point");
		return NULL;  // Incorrect formatting		
	}
	state->koPoint = atoi(line);

	// Anything afterwards is a comment, and should be ignored

	fclose(fp);	

	return state;
}

void serializeState(State *state, char *fileName) {
	FILE *fp = fopen(fileName, "w");

	if (fp == NULL) {
		ERROR_PRINT("Couldn't find file: %s", fileName);
		exit(1);
	}

	for (int i = BOARD_DIM-1; i >= 0; i--) {
		char line[BOARD_DIM+1];

		for (int j = 0; j < BOARD_DIM; j++) {
			int point = i*BOARD_DIM+j;

			switch (state->board[point]) {
				case STATE_WHITE:
					line[j] = 'W';
					break;
				case STATE_BLACK:
					line[j] = 'B';
					break;
				case STATE_EMPTY:
					line[j] = '-';
					break;
				default:
					ERROR_PRINT("Unknown character in serializing: %d at point %d", state->board[point], point);
					exit(1);
			}
		}
		line[BOARD_DIM] = '\0';

		fprintf(fp, "%s\n", line);
	}

	fprintf(fp, "%c\n", state->turn == STATE_WHITE ? 'W' : 'B');
	fprintf(fp, "%d\n", state->whitePrisoners);
	fprintf(fp, "%d\n", state->blackPrisoners);
	fprintf(fp, "%d\n", state->koPoint);


	fclose(fp);
}

int parseMove(char *fileName) {
	FILE *fp = fopen(fileName, "r");

	if (fp == NULL) {
		ERROR_PRINT("Couldn't find file: %s", fileName);
		exit(1);
	}

	char line[MAX_MOVE_LEN];

	if (fscanf(fp, "%s", line) == EOF) {
		ERROR_PRINT("Move is missing");
		return -2;
	}	

	if (line[0] != 'B' && line[1] != 'W') {
		ERROR_PRINT("Failed to specify player to move");
		return -2;
	}

	if (line[1] != '[') {
		ERROR_PRINT("Move formatting error");
		return -2;
	}

	if (line[2] == ']') {
		return MOVE_PASS;  // Pass
	}

	char colChar = line[2];
	char rowChar = line[3];

	if (rowChar == 't' && colChar == 't') {
		return MOVE_PASS;  // Also pass
	}

	int point = BOARD_DIM*(rowChar-'a') + colChar-'a';

	fclose(fp);

	return point;
}