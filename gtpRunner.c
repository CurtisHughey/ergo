#include "gtpRunner.h"

int runGtp(int rollouts, int lengthOfGame) {
	// First set up go board

	srand(time(NULL));	

	// Add undo, kgs-game_over, time ^^
	// Need to account for comments ^^^

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
						};

	const char *whitespace = " \t";

	State *state = createState();

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
				strncat(response, commands[i], strlen(commands[i])+1);

				if (i < numCommands-1) {  // Bit hacky, TBH.  Need to append new line if more commands
					strncat(response, "\n", 2);
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
			// Will also eventually have to remove other state info ^^
			// No output
		} else if (!strcmp(command, "komi")) {
			char *komiString = strtok(NULL, whitespace);
			if (komiString == NULL) {
				sprintf(errorMessage, "syntax error, komi not specified");
				goto ERROR;
			}			
			float newKomi = atof(komiString);
			UNUSED(newKomi);  // CHANGE!!! ^^^^
			// No output
		} else if (!strcmp(command, "play")) {

			// Color

			char *colorString = strtok(NULL, whitespace);
			if (colorString == NULL) {
				sprintf(errorMessage, "syntax error, color not specified");
				goto ERROR;
			}

			int color = stringColorToInt(colorString);
			if (color == 0) {
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

			if (move == INVALID_MOVE) {
				sprintf(errorMessage, "syntax error, invalid move");
				goto ERROR;
			}

			if (!isLegalMove(state, move)) {
				sprintf(errorMessage, "illegal move");
				goto ERROR;
			}
			////////////////

			state->turn = color;  // Potentially changes it (I guess if we were setting up positions).  This is done at the end in case there were prior errors
			makeMove(state, move);  // At some stage, will want to be able to unmake
			// No output
		} else if (!strcmp(command, "genmove")) {
			char *colorString = strtok(NULL, whitespace);
			if (colorString == NULL) {
				sprintf(errorMessage, "syntax error, vertex not specified");
				goto ERROR;
			}
			
			int color = stringColorToInt(colorString);

			compColor = color;  // Used for scoring

			if (color == 0) {
				sprintf(errorMessage, "syntax error, invalid color");
				goto ERROR;  // Technically, I shouldn't error out, according to the spec ^^^
			}
			
			state->turn = color;

			int move = uctSearch(state, rollouts, lengthOfGame);
			makeMove(state, move);  // Again, give ability to unmake ^^^

			char *vertex = moveToGtpString(move);  // Can also resign ^^

			sprintf(response, vertex);
			
			free(vertex);
			vertex = NULL;
		} else if (!strcmp(command, "kgs-rules")) {
			char *rules = strtok(NULL, whitespace);
			UNUSED(rules);  // Might be useful ^^^	
		} else if (!strcmp(command, "kgs-time_settings")) {
			// Ignoring right now, implement! ^^^
		} else if (!strcmp(command, "time_left")) {
			// Ignoring right now, implement! ^^^
		} else if (!strcmp(command, "kgs-game_over")) {
			quit = 1;  // I guess? ^^^
			finished = 1;
			// No output
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
			destroyState(state);
			fclose(fp);

			if (finished) {
				return getResult(state, compColor);
			} else {
				return -2;  // Unfinished
			}
		}

		//displayState(state);  // Remove ^^^

		continue;

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
		return INVALID_MOVE;
	}

	// First parse column
	int column;

	const char highestColumnUpperCase = BOARD_DIM > 'I'-'A' ? 'A'+BOARD_DIM-1 : 'A'+BOARD_DIM;  // I think I got this right ^^^
	const char highestColumnLowerCase = BOARD_DIM > 'i'-'a' ? 'a'+BOARD_DIM : 'a'+BOARD_DIM;

	char columnChar = vertex[0];

	// I could probably call a lib function
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
		return INVALID_MOVE;
	}

	////////

	// Now parse row
	int row = 0;

	int tens = 0;  // tens digit (might not be present)
	int ones = 0;  // ones digit

	if (!isdigit(vertex[1])) {
		return INVALID_MOVE;
	} else {
		tens = vertex[1]-'0';  // Initally guess that this is the tens index

		if (tens < 0 || tens > 9) {
			return INVALID_MOVE;
		}
	}

	// Same thing with ones
	if (vertex[2] != '\0') {
		if (!isdigit(vertex[2])) {
			return INVALID_MOVE;
		} else {
			ones = vertex[2]-'0';

			if (ones < 0 || ones > 9) {
				return INVALID_MOVE;
			}
		}
	} else {  // Switches it
		ones = tens;
		tens = 0;
	}

	row = ones+10*tens;

	if (row < 0 || row > BOARD_DIM) {
		return INVALID_MOVE;
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
	} else {
		char columnChar = 'A'+ move % BOARD_DIM;
		if (columnChar >= 'I') {
			columnChar += 1;  // Incrementing
		}

		int row = move / BOARD_DIM + 1;  // Makes it [1-BOARD_DIM]

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