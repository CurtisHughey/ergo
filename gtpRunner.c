#include "gtpRunner.h"

void runGtp(int rollouts, int lengthOfGame) {
	// First set up go board

	srand(time(NULL));	

	State *state = createState();

	///////////////


	// Add undo, kgs-game_over, time ^^
	// Need to account for comments ^^^

 	const char *commands[] = { "protocol_version",  // Maybe dynamically generate from below ^^^
							"list_commands", 
							"known_command"
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
						};

	const char *whitespace = " \t";

	int quit = 0;
	char inBuffer[GTP_MAX_LENGTH];
	while (fgets(inBuffer, GTP_MAX_LENGTH, stdin)) {
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
				sprintf(errorMessage, "sytax error, command not specified");
				goto ERROR;
			}
			sprintf(returnId, "=%s", token);
		} else {
			command = token;
			sprintf(returnId, "=");;  // No id
		}
		
		// Huge parser
		if (!strncmp(command, "protocol_version", GTP_MAX_LENGTH)) {
			sprintf(response, "%d", GTP_PROTOCOL_VERSION);
		} else if (!strncmp(command, "list_commands", GTP_MAX_LENGTH)) {
			for (int i = 0; i < numCommands; i++) {
				strncat(response, commands[i], strlen(commands[i])+1);

				if (i < numCommands-1) {  // Bit hacky, TBH.  Need to append new line if more commands
					strncat(response, "\n", 2);
				}
			}
		} else if (!strncmp(command, "known_command", GTP_MAX_LENGTH)) {
			char *subCommand = strtok(NULL, whitespace);
			if (subCommand == NULL) {
				sprintf(errorMessage, "sytax error, sub command not specified");
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
		} else if (!strncmp(command, "name", GTP_MAX_LENGTH)) {
			sprintf(response, "%s", GTP_NAME);
		} else if (!strncmp(command, "version", GTP_MAX_LENGTH)) {
			sprintf(response, "%s", GTP_NAME);
		} else if (!strncmp(command, "quit", GTP_MAX_LENGTH)) {
			quit = 1; 
			// No output
		} else if (!strncmp(command, "boardsize", GTP_MAX_LENGTH)) {
			char *sizeString = strtok(NULL, whitespace);
			if (sizeString == NULL) {
				sprintf(errorMessage, "sytax error, size not specified");
				goto ERROR;
			}
			int newSize = atoi(strtok(NULL, whitespace));  // Must be a int

			if (newSize != BOARD_DIM) {  // Can't change given our program, since BOARD_DIM is defined
				sprintf(errorMessage, "unacceptable size");
			}
			// No output
		} else if (!strncmp(command, "clear_board", GTP_MAX_LENGTH)) {
			clearBoard(state);
			// Will also eventually have to remove other state info ^^
			// No output
		} else if (!strncmp(command, "komi", GTP_MAX_LENGTH)) {
			char *komiString = strtok(NULL, whitespace);
			if (komiString == NULL) {
				sprintf(errorMessage, "sytax error, komi not specified");
				goto ERROR;
			}			
			float newKomi = atof(komiString);
			UNUSED(newKomi);  // CHANGE!!! ^^^^
			// No output
		} else if (!strncmp(command, "play", GTP_MAX_LENGTH)) {

			// Color
			const int maxColorLength = 10;  // High

			char *colorString = strtok(NULL, whitespace);
			if (colorString == NULL) {
				sprintf(errorMessage, "sytax error, color not specified");
				goto ERROR;
			}

			int color;

			if (!strncasecmp(colorString, "white", maxColorLength) || !strncasecmp(colorString, "w", maxColorLength)) {
				color = STATE_WHITE;
			} else if (!strncasecmp(colorString, "black", maxColorLength) || !strncasecmp(colorString, "b", maxColorLength)) {
				color = STATE_BLACK;
			} else {
				sprintf(errorMessage, "syntax error, invalid color");
				goto ERROR;
			}
			
			state->turn = color;  // Potentially changes it (I guess if we were setting up positions)
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
			printf("MOVE: %d\n", move);
			//makeMove(state, move);  // At some stage, will want to be able to unmake
			// No output
		}

		else {
			sprintf(errorMessage, "unknown command");
			goto ERROR;
		}		


		// This goto stuff is a little wonky, especially with the continue
		fprintf(stdout, "%s %s\n\n", returnId, response);  // Writes it back
		if (quit) {  // quitting early, whatever
			// Free stuff ^^^
			return;
		}
		continue;

ERROR:
		returnId[0] = '?';  // Now an error message
		fprintf(stdout, "%s %s\n\n", returnId, errorMessage);  // Writes it back
	}
}

int parseGtpMove(char *vertex) {
	if (!strncasecmp(vertex, "pass", 5)) {
		return MOVE_PASS;
	}

	if (strlen(vertex) < 2) {  // Too small, must either be length 3 or 4
		return INVALID_MOVE;
	}

	// First parse column
	int column;

	const char highestColumnUpperCase = BOARD_DIM > 'I'-'A' ? 'A'+BOARD_DIM : 'A'+BOARD_DIM-1;  // I think I got this right ^^^
	const char highestColumnLowerCase = BOARD_DIM > 'i'-'a' ? 'a'+BOARD_DIM : 'a'+BOARD_DIM-1;

	char columnChar = vertex[0];

	// I could probably call a lib function
	if (columnChar >= 'A' && columnChar <= highestColumnUpperCase) {
		column = columnChar-'A';
		if (columnChar >= 'J') {
			column += 1;  // Correcting the skipped I
		}
	} else if (columnChar >= 'a' && columnChar <= highestColumnLowerCase) {
		column = columnChar-'a';
		if (columnChar >= 'j') {
			column += 1;  // Correcting the skipped j
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
		ones = vertex[1]-'1';

		if (ones < 0 || ones > 9) {
			return INVALID_MOVE;
		}
	}

	// Same thing with tens
	if (vertex[2] != '\0') {
		if (!isdigit(vertex[2])) {
			return INVALID_MOVE;
		} else {
			tens = vertex[2]-'1';

			if (tens < 0 || tens > 9) {
				return INVALID_MOVE;
			}
		}
	}

	row = ones+10*tens;

	if (row <= 0 || row >= BOARD_DIM) {
		return INVALID_MOVE;
	}

	////////

	int position = column+row*BOARD_DIM;

	return position;
}