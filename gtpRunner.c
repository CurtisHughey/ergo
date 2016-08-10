#include "gtpRunner.h"

int runGtp(Config *config) {
	srand(time(NULL));	

	// Add undo, kgs-game_over, time ^^

 	const char *commands[] = { "protocol_version",  // Maybe dynamically generate from below ^^^
							"list_commands", 
							"known_command",
							"name", 
							"version",
							"quit",
							"boardsize",
							"clear_board",
							"komi",
							"play",
							"genmove",
							"kgs-rules",
							"kgs-time_settings",
							"kgs-game_over",
							"reg_genmove",
						};

	const char *whitespace = " \t";

	//  Set up go board
	State *state = createState();
	HashTable *hashTable = createHashTable(config->hashBuckets);

	///////////////

	FILE *fp = fopen(LOG_FILE_NAME, "a");
	if (fp == NULL) {
		ERROR_PRINT("Failed to open file %s", LOG_FILE_NAME);
		return -2;
	}

	fprintf(fp, "--------------------\n");
	fflush(fp);

	int quit = 0;
	int finished = 0;
	int compColor = 0;
	char inBuffer[GTP_MAX_LENGTH];
	while (fgets(inBuffer, GTP_MAX_LENGTH, stdin)) {
		fprintf(fp, "> %s\n", inBuffer);
		fflush(fp);

		if (strlen(inBuffer) == 0) {  // Nothing, probably won't happen
			continue;
		} else {  // Must get rid of newlines at the end of the string
			for (int i = strlen(inBuffer)-1; inBuffer[i] == '\n'; i--) {
				inBuffer[i] = '\0';
			}
		}

		char returnId[GTP_MAX_LENGTH] = "";  // =[id] (obviously is wayyy smaller than GTP_MAX_LENGTH)
		char *command;  // The command read in

		char response[GTP_MAX_LENGTH] = "";
		char errorMessage[GTP_MAX_LENGTH] = "";

		char *token = strtok(inBuffer, whitespace);

		if (token == NULL) {  // Ignore, was just whitespace/newline
			continue;
		}

		if (token[0] == '#') {  // Line starts with a comment, ignore.  Other comments are implicitly ignored at the ends of lines below (we stop parsing tokens)
			continue;
		}

		if (isdigit(token[0])) {  // Then id was sent (assuming there are no commands that start with number)
			command = strtok(NULL, whitespace);
			if (command == NULL) {
				sprintf(errorMessage, "syntax error, command not specified");
				goto ERROR;
			}
			sprintf(returnId, "=%s", token);
		} else {
			command = token;
			sprintf(returnId, "=");;  // No id
		}
		
		// Huge parser
		if (!strcmp(command, "protocol_version")) {
			sprintf(response, "%d", GTP_PROTOCOL_VERSION);
		} else if (!strcmp(command, "list_commands")) {
			for (int i = 0; i < numCommands; i++) {
				strcat(response, commands[i]);

				if (i < numCommands-1) {  // Bit hacky, TBH.  Need to append new line if more commands
					strcat(response, "\n");
				}
			}
		} else if (!strcmp(command, "known_command")) {
			char *subCommand = strtok(NULL, whitespace);
			if (subCommand == NULL) {
				sprintf(errorMessage, "syntax error, sub command not specified");
				goto ERROR;
			}
			int found = 0;

			for (int i = 0; i < numCommands; i++) {
				if (strncmp(subCommand, commands[i], GTP_MAX_LENGTH)) {
					sprintf(response, "true");
					found = 1;
				}
			}

			if (!found) {
				sprintf(response, "false");
			}
		} else if (!strcmp(command, "name")) {
			sprintf(response, "%s", GTP_NAME);
		} else if (!strcmp(command, "version")) {
			sprintf(response, "%s", GTP_VERSION);
		} else if (!strcmp(command, "quit")) {
			quit = 1; 
			// No output
		} else if (!strcmp(command, "boardsize")) {
			char *sizeString = strtok(NULL, whitespace);
			if (sizeString == NULL) {
				sprintf(errorMessage, "syntax error, size not specified");
				goto ERROR;
			}
			int newSize = atoi(sizeString);  // Must be a int

			if (newSize != BOARD_DIM) {  // Can't change given our program, since BOARD_DIM is defined
				sprintf(errorMessage, "unacceptable size");
				goto ERROR;
			}
			// No output
		} else if (!strcmp(command, "clear_board")) {
			clearBoard(state);
			// Would also have to remove other state info at this point
			// No output
		} else if (!strcmp(command, "komi")) {
			char *komiString = strtok(NULL, whitespace);
			if (komiString == NULL) {
				sprintf(errorMessage, "syntax error, komi not specified");
				goto ERROR;
			}			
			float newKomi = atof(komiString);
			setKomi(newKomi);
			// No output
		} else if (!strcmp(command, "play")) {
			// Color
			char *colorString = strtok(NULL, whitespace);
			if (colorString == NULL) {
				sprintf(errorMessage, "syntax error, color not specified");
				goto ERROR;
			}

			int color = stringColorToInt(colorString);
			if (color != STATE_WHITE && color != STATE_BLACK) {
				sprintf(errorMessage, "syntax error, invalid color");
				goto ERROR;
			}
			
			////////////////
			// Move

			char *vertexString = strtok(NULL, whitespace);
			if (vertexString == NULL) {
				sprintf(errorMessage, "sytax error, vertex not specified");
				goto ERROR;
			}

			int move;

			move = parseGtpMove(vertexString);

			if (move == MOVE_INVALID) {
				sprintf(errorMessage, "syntax error, invalid move");

				goto ERROR;
			}

			int prevColor = state->turn;
			state->turn = color;  // Potentially changes the color (usually if setting up board)
			if (!isLegalMove(state, move, hashTable)) {  // Not sure how much we insist it's an illegal move if superko is involved, might be better to just defer ^^^
				sprintf(errorMessage, "illegal move");
				state->turn = prevColor;  // Resets
				goto ERROR;
			}
			////////////////

			makeMove(state, move, hashTable);  // At some stage, will want to be able to unmake
			// No output
		} else if (!strcmp(command, "genmove")) {
			char *colorString = strtok(NULL, whitespace);
			if (colorString == NULL) {
				sprintf(errorMessage, "syntax error, vertex not specified");
				goto ERROR;
			}
			
			int color = stringColorToInt(colorString);

			if (color != STATE_WHITE && color != STATE_BLACK) {
				sprintf(errorMessage, "syntax error, invalid color");
				goto ERROR;  // We choose to error out, even against the spec
			}

			compColor = color;  // Used for scoring			
			state->turn = color;  // Might change, if loading position

			int move = uctSearch(state, config, hashTable);
			if (move != MOVE_RESIGN) {
				makeMove(state, move, hashTable);  // Again, give ability to unmake ^^^
			}

			char *vertex = moveToGtpString(move);  // Can also resign ^^

			sprintf(response, vertex);
			
			free(vertex);
			vertex = NULL;
		} else if (!strcmp(command, "kgs-rules")) {
			char *rules = strtok(NULL, whitespace);
			UNUSED(rules);  // Might be useful in the future
			// No output
		} else if (!strcmp(command, "kgs-time_settings")) {
			// Ignoring right now, I don't care what time settings for the moment, but eventually change, with time_left
			// No output
		} else if (!strcmp(command, "time_left")) {
			// Ignoring right now, implement when I start seriously caring about time
			// No output
		} else if (!strcmp(command, "kgs-game_over")) {
			quit = 1;
			finished = 1;
			// No output
		} else if (!strcmp(command, "reg_genmove")) {
			// Identical to genmove, apart from actually making the move
			char *colorString = strtok(NULL, whitespace);
			if (colorString == NULL) {
				sprintf(errorMessage, "syntax error, vertex not specified");
				goto ERROR;
			}
			
			int color = stringColorToInt(colorString);

			if (color != STATE_WHITE && color != STATE_BLACK) {
				sprintf(errorMessage, "syntax error, invalid color");
				goto ERROR;  // We choose to error out, even against the spec
			}

			compColor = color;  // Used for scoring			
			state->turn = color;

			int move = uctSearch(state, config, hashTable);
			char *vertex = moveToGtpString(move);  // Could be resignation

			sprintf(response, vertex);
			
			free(vertex);
			vertex = NULL;			
		} 
		else {
			sprintf(errorMessage, "unknown command");
			goto ERROR;
		}

		// This goto stuff is a little wonky, especially with the continue
		fprintf(stdout, "%s %s\n\n", returnId, response);  // Writes it back
		fflush(stdout);
		fprintf(fp, "< %s %s\n\n", returnId, response);
		fflush(fp);


		if (quit) {  // quitting early, whatever
			destroyHashTable(hashTable);
			destroyState(state);
			fclose(fp);

			if (finished) {
				return getResult(state, compColor);
			} else {
				return -2;  // Unfinished with the game, still quitting
			}
		}

		// Reads the next line of input
		continue;

		// Error time!
ERROR:
		returnId[0] = '?';  // Now an error message
		fprintf(stdout, "%s %s\n\n", returnId, errorMessage);  // Writes it back
		fflush(stdout);
		fprintf(fp, "< %s %s\n\n", returnId, errorMessage);
		fflush(fp);
	}

	return -2;  // Should never end up here
}

int parseGtpMove(char *vertex) {
	if (!strcasecmp(vertex, "pass")) {
		return MOVE_PASS;
	}

	if (strlen(vertex) < 2) {  // Too small, must either be length 3 or 4
		return MOVE_INVALID;
	}

	// First parse column
	int column;

	const char highestColumnUpperCase = BOARD_DIM > 'I'-'A' ? 'A'+BOARD_DIM : 'A'+BOARD_DIM;
	const char highestColumnLowerCase = BOARD_DIM > 'i'-'a' ? 'a'+BOARD_DIM : 'a'+BOARD_DIM;

	char columnChar = vertex[0];

	// Return error if i/I
	if (columnChar >= 'A' && columnChar <= highestColumnUpperCase) {
		column = columnChar-'A';
		if (columnChar >= 'J') {
			column -= 1;  // Correcting the skipped I
		}
	} else if (columnChar >= 'a' && columnChar <= highestColumnLowerCase) {
		column = columnChar-'a';
		if (columnChar >= 'j') {
			column -= 1;  // Correcting the skipped j
		}
	} else {
		return MOVE_INVALID;
	}

	////////
	// This ended up being really wonky code.  Essentialy I'm parsing the two digits, while ensuring each are valid
	// Now parse row
	int row = 0;

	int tens = 0;  // tens digit (might not be present)
	int ones = 0;  // ones digit

	if (!isdigit(vertex[1])) {
		return MOVE_INVALID;
	} else {
		tens = vertex[1]-'0';  // Initally guess that this is the tens index

		if (tens < 0 || tens > 9) {
			return MOVE_INVALID;
		}
	}

	// Same thing with ones
	if (vertex[2] != '\0') {
		if (!isdigit(vertex[2])) {
			return MOVE_INVALID;
		} else {
			ones = vertex[2]-'0';

			if (ones < 0 || ones > 9) {
				return MOVE_INVALID;
			}
		}
	} else {  // Switches it
		ones = tens;
		tens = 0;
	}

	row = ones+10*tens;

	if (row < 0 || row > BOARD_DIM) {
		return MOVE_INVALID;
	}

	////////

	int position = column+(row-1)*BOARD_DIM;

	return position;
}

// Just the vertex
char *moveToGtpString(int move) {
	char *moveString = calloc(MAX_GTP_MOVE_LENGTH, sizeof(char));

	if (move == MOVE_PASS) {
		sprintf(moveString, "pass");
	} else if (move == MOVE_RESIGN) {
		sprintf(moveString, "resign");
	} else {
		char columnChar = 'A'+ move % BOARD_DIM;
		if (columnChar >= 'I') {
			columnChar += 1;  // Incrementing
		}

		int row = move / BOARD_DIM + 1;  // Makes it range [1-BOARD_DIM]

		sprintf(moveString, "%c%d", columnChar, row);
	}

	return moveString;
}

int stringColorToInt(char *colorString) {
	int color;

	if (!strcasecmp(colorString, "white") || !strcasecmp(colorString, "w")) {
		color = STATE_WHITE;
	} else if (!strcasecmp(colorString, "black") || !strcasecmp(colorString, "b")) {
		color = STATE_BLACK;
	} else {
		color = 0;  // Error
	}

	return color;
}