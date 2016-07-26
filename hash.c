#include "hash.h"

// This function uses strtoull
void initHashVals(void) {
	FILE *fp = fopen(RANDOM_NUMBER_FILE, "r");
	if (fp == NULL) {
		ERROR_PRINT("Failed to open random number file: %s", RANDOM_NUMBER_FILE);
		exit(1);
	}

	int bufSize = sizeof(HASHVALUETYPE) * 2 + 2;  // Number of nibbles for 64 bits (and newline and null terminator)
	char *inBuffer = calloc(bufSize, sizeof(char));
	
	for (int i = 0; i < BOARD_SIZE; i++) {

		char *blackValString = fgets(inBuffer, bufSize, fp);  // Reads in the appropriate number of hex nibbles
		if (blackValString == NULL) {
			ERROR_PRINT("Not enough random numbers, at iteration %d\n", i);
			exit(1);
		}		
		char *whiteValString = fgets(inBuffer, bufSize, fp);
		if (whiteValString == NULL) {
			ERROR_PRINT("Not enough random numbers, at iteration %d\n", i);
			exit(1);
		}
		char *emptyValString = fgets(inBuffer, bufSize, fp);
		if (emptyValString == NULL) {
			ERROR_PRINT("Not enough random numbers, at iteration %d\n", i);
			exit(1);
		}

		HASHVALUETYPE blackVal = strtoull(blackValString, (char **)NULL, 16);
		HASHVALUETYPE whiteVal = strtoull(whiteValString, (char **)NULL, 16);  // Base 16
		HASHVALUETYPE emptyVal = strtoull(emptyValString, (char **)NULL, 16);

		blackVals[i] = blackVal;
		whiteVals[i] = whiteVal;   // Globals defined in hash.h
		emptyVals[i] = emptyVal;
	}

	char *koValString = fgets(inBuffer, bufSize, fp);
	if (koValString == NULL) {
		ERROR_PRINT("Not enough random numbers, at koVal");
		exit(1);
	}

	koVal = strtoull(koValString, (char **)NULL, 16);  // Global defined in hash.h

	free(inBuffer);
	inBuffer = NULL;
	fclose(fp);
}

HASHVALUETYPE zobristHash(State *state) {
	HASHVALUETYPE result = 0;

	for (int i = 0; i < BOARD_SIZE; i++) {
		switch (state->board[i]) {
			case STATE_BLACK:
				result ^= blackVals[i];
				break;
			case STATE_WHITE:
				result ^= whiteVals[i];
				break;
			case STATE_EMPTY:
				result ^= emptyVals[i];
				break;
			default:
				ERROR_PRINT("Unknown piece on board: %d", i);  // Just ignores, I guess
				break;
		}
	}

	if (state->koPoint >= 0) {
		result ^= koVal;
	}

	return result;
}

// Creates a hash table with numBuckets buckets (linked lists)
HashTable *createHashTable(int numBuckets);

// Destroys the hash table
HashTable *destroyHashTable(HashTable *hashTable);

// Adds a state to the hash table.  Returns 0 upon success
int addToHashTable(HashTable *hashTable, State *state);

// Deletes a state to the hash table.  Returns 0 upon success
int deleteFromHashTable(HashTable *hashTable, State *state);

// Sees if a state is contained in the hash table.  Returns 1 if true, 0 if not
int containsInHashTable(HashTable *hashTable, State *state);

// Returns the number of hashed values in the hash table
int sizeOfHashTable(HashTable *hashTable, State *state);
