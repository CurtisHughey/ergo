#include "gameParser.h"

// Parses line by line, each point is either W, B, or space (" ")
// Then some extra metadata (turn, ko, etc.)
State *parseState(char *fileName) {
	State *state = createState(0);  // No superko!!! (this sort of makes sense since there is no info about previous moves

	FILE *fp = fopen(fileName, "r");

	if (fp == NULL) {
		ERROR_PRINT("Couldn't find file: %s", fileName);
		exit(1);
	}

	char line[MAX_BOARD_LINE];

	for (int i = BOARD_DIM-1; i >= 0; i--) {
		if (fgets(line, MAX_BOARD_LINE, fp) == NULL) {
			ERROR_PRINT("Not enough lines: %s", fileName);
			return NULL;  // Incorrect formatting
		}

		if (i == BOARD_DIM-1 && line[0] == '#') {  // Detecting comments, but hacky since doesn't allow whitespace
			do {
				if (fgets(line, MAX_BOARD_LINE, fp) == NULL) {
					ERROR_PRINT("Not enough lines: %s", fileName);
					return NULL;  // Incorrect formatting
				}
			} while (line[0] == '#');
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
					ERROR_PRINT("Not enough on a line: %s", fileName);
					return NULL;  // Error
				default:
					ERROR_PRINT("Illegal character: %c at %d: %s", line[j], point, fileName);
					return NULL;
			}
		}
	}
	// Next line should be turn (either W or B)
	if (fgets(line, MAX_BOARD_LINE, fp) == NULL) {
		ERROR_PRINT("Missing turn: %s", fileName);
		return NULL;  // Incorrect formatting		
	}
	state->turn = line[0] == 'W' ? STATE_WHITE : STATE_BLACK;

	// White stones captured (number)
	if (fgets(line, MAX_BOARD_LINE, fp) == NULL) {
		ERROR_PRINT("Missing white prisoners: %s", fileName);
		return NULL;  // Incorrect formatting		
	}
	state->whitePrisoners = atoi(line);

	// White stones captured (number)
	if (fgets(line, MAX_BOARD_LINE, fp) == NULL) {
		ERROR_PRINT("Missing black prisoners: %s", fileName);
		return NULL;  // Incorrect formatting		
	}
	state->blackPrisoners = atoi(line);

	// Ko point (-1 if none)	
	if (fgets(line, MAX_BOARD_LINE, fp) == NULL) {
		ERROR_PRINT("Missing ko point: %s", fileName);
		return NULL;  // Incorrect formatting		
	}
	state->koPoint = atoi(line);

	// Whether black has passed or not (used to help determine end of game)
	if (fgets(line, MAX_BOARD_LINE, fp) == NULL) {
		ERROR_PRINT("Missing black passed: %s", fileName);
		return NULL;  // Incorrect formatting		
	}
	state->blackPassed = atoi(line);

	state->komi = DEFAULT_KOMITIMES10 / 10.0;  // Hmm...  do I want this to be parseable?

	fclose(fp);	

	return state;
}

void serializeState(State *state, char *fileName, int append) {
	FILE *fp = NULL;
	if (append) {
		fp = fopen(fileName, "a");
		fprintf(fp, "------------------------------\n");
	} else {
		fp = fopen(fileName, "w");
	}

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
	fprintf(fp, "%d\n", state->blackPassed);

	fclose(fp);
}

int parseMoveFromFile(char *fileName) {
	FILE *fp = fopen(fileName, "r");

	if (fp == NULL) {
		ERROR_PRINT("Couldn't find file: %s", fileName);
		exit(1);
	}

	char line[MAX_MOVE_LEN];

	if (fgets(line, MAX_MOVE_LEN, fp) == NULL) {
		ERROR_PRINT("Move is missing");
		return MOVE_INVALID;
	}

	fclose(fp);

	return parseMove(line);
}

int parseMoveFromTerminal(void) {
	char line[MAX_MOVE_LEN];
	fgets(line, MAX_MOVE_LEN, stdin);

	return parseMove(line);
}

int parseMove(char *line) {
	if (line[0] == 'R') {  // Special resign command
		return MOVE_RESIGN;
	}

	if (line[0] != 'B' && line[0] != 'W') {
		ERROR_PRINT("Failed to specify player to move");
		return MOVE_INVALID;
	}

	if (line[1] != '[') {
		ERROR_PRINT("Missing opening square bracket");
		return MOVE_INVALID;
	}

	if (line[2] == ']') {
		return MOVE_PASS;  // Pass
	}

	char colChar = line[2];
	char rowChar = line[3];

	if (colChar == '\0' || rowChar == '\0') {
		ERROR_PRINT("Failed to specify row or column");
		return MOVE_INVALID;		
	}

	if (line[4] != ']') {
		ERROR_PRINT("Missing closing square bracket");
		return MOVE_INVALID;
	}

	if (rowChar == 't' && colChar == 't') {
		return MOVE_PASS;  // Also pass
	}

	if (rowChar < 'a' || rowChar >= 'a'+BOARD_DIM) {
		ERROR_PRINT("Invalid row entry");
		return MOVE_INVALID;
	}

	if (colChar < 'a' || colChar >= 'a'+BOARD_DIM) {
		ERROR_PRINT("Invalid column entry");
		return MOVE_INVALID;
	}

	int point = BOARD_DIM*(rowChar-'a') + colChar-'a';

	return point;
}

char *moveToString(int move, int color) {
	char *moveString = calloc(MAX_MOVE_LEN, sizeof(char));  // 0's out to be safe

	char colorChar = color == STATE_BLACK ? 'B' : 'W';

	if (move == MOVE_PASS) {
		sprintf(moveString, "%c[]", colorChar);
	} else if (move == MOVE_RESIGN) {
		sprintf(moveString,  "RESIGN");  // I guess
	} else if (move >= 0 && move < BOARD_SIZE) {
		int column = move % BOARD_DIM;
		int row = move / BOARD_DIM;

		int columnChar = column+'a';
		int rowChar = row+'a';

		sprintf(moveString, "%c[%c%c]", colorChar, columnChar, rowChar);
	} else {
		ERROR_PRINT("Invalid move, continuing");
		sprintf(moveString, "BAD");
	}

	return moveString;
}
