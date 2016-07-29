#include "hash.h"

// ^^^ Consider storing the hash value of a position in state, and then when we make a move figure out what things need to get xored 

// This function uses strtoull
void initHashVals(char *fileName) {
	if (fileName == NULL) {
		fileName = DEFAULT_RANDOM_NUMBER_FILE;
	}

	FILE *fp = fopen(fileName, "r");
	if (fp == NULL) {
		ERROR_PRINT("Failed to open random number file: %s", fileName);
		exit(1);
	}

	int bufSize = sizeof(HASHVALUETYPE) * 2 + 2;  // Number of nibbles for 64 bits (and newline and null terminator)
	char *inBuffer = calloc(bufSize, sizeof(char));
	
	// Reads in the string numbers, converts to HASHVALUETYPE's, and stores
	for (int i = 0; i < BOARD_SIZE; i++) {

		char *blackValString = fgets(inBuffer, bufSize, fp);  // Reads in the appropriate number of hex nibbles
		if (blackValString == NULL) {
			ERROR_PRINT("Not enough random numbers, at iteration %d\n", i);
			exit(1);
		}	
		HASHVALUETYPE blackVal = strtoull(blackValString, (char **)NULL, 16);	
		
		char *whiteValString = fgets(inBuffer, bufSize, fp);
		if (whiteValString == NULL) {
			ERROR_PRINT("Not enough random numbers, at iteration %d\n", i);
			exit(1);
		}
		HASHVALUETYPE whiteVal = strtoull(whiteValString, (char **)NULL, 16);  // Base 16

		char *emptyValString = fgets(inBuffer, bufSize, fp);
		if (emptyValString == NULL) {
			ERROR_PRINT("Not enough random numbers, at iteration %d\n", i);
			exit(1);
		}
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

// Should do a check to make sure initHashValues was called ^^^
HashTable *createHashTable(int numBuckets) {
	if (numBuckets < 1) {
		return NULL;  // Not keeping track of superko
	}

	Node ***buckets = calloc(numBuckets, sizeof(Node**));  // Geez, triple pointers.  First level allows head to be modified, second level allows for allocating bucket on heap, third level allows for array of buckets
	if (buckets == NULL) {
		ERROR_PRINT("Failed to allocate buckets");
		exit(1);
	}

	for (int i = 0; i < numBuckets; i++) {
		Node **bucket = malloc(sizeof(Node*));
		if (bucket == NULL) {
			ERROR_PRINT("Failed to allocate bucket");
			exit(1);
		}
		*bucket = NULL;  // Must initialize it to this (maybe logic should be done in linkedList.c)  (should this be done?)

		buckets[i] = bucket;	
	}

	HashTable *hashTable = malloc(sizeof(HashTable));

	hashTable->buckets = buckets;
	hashTable->numBuckets = numBuckets;

	return hashTable;
}

void destroyHashTable(HashTable *hashTable) {
	if (hashTable != NULL) {  // Might be NULL if we weren't keeping track of superko
		for (int i = 0; i < hashTable->numBuckets; i++) {
			listClear(hashTable->buckets[i]);
			free(hashTable->buckets[i]);
			hashTable->buckets[i] = NULL;
		}

		free(hashTable->buckets);
		hashTable->buckets = NULL;

		free(hashTable);
		hashTable = NULL;
	}
}

void clearHashTable(HashTable *hashTable) {
	if (hashTable == NULL) {
		return;
	}

	// All we have to do is clear out entries in the buckets
	for (int i = 0; i < hashTable->numBuckets; i++) {
		listClear(hashTable->buckets[i]);
	}
}

int addToHashTable(HashTable *hashTable, State *state) {
	if (hashTable == NULL) {
		ERROR_PRINT("Hash Table uninitialized");
		return 1;
	}

	HASHVALUETYPE hashValue = zobristHash(state);
	int bucketIndex = (int)(hashValue % hashTable->numBuckets);  // Casting ^^^

	return listAdd(hashTable->buckets[bucketIndex], hashValue);
}

int deleteFromHashTable(HashTable *hashTable, State *state) {
	if (hashTable == NULL) {
		ERROR_PRINT("Hash Table uninitialized");
		return 1;
	}

	HASHVALUETYPE hashValue = zobristHash(state);
	int bucketIndex = (int)(hashValue % hashTable->numBuckets);

	return listDelete(hashTable->buckets[bucketIndex], hashValue);
}

// This should hook into the previous function
int deleteValueFromHashTable(HashTable *hashTable, HASHVALUETYPE hashValue) {
	if (hashTable == NULL) {
		ERROR_PRINT("Hash Table uninitialized");
		return 1;
	}

	int bucketIndex = (int)(hashValue % hashTable->numBuckets);

	return listDelete(hashTable->buckets[bucketIndex], hashValue);
}

int containsInHashTable(HashTable *hashTable, State *state) {
	if (hashTable == NULL) {
		ERROR_PRINT("Hash Table uninitialized");
		return 0;  // I guess?
	}

	HASHVALUETYPE hashValue = zobristHash(state);
	int bucketIndex = (int)(hashValue % hashTable->numBuckets);

	return listContains(hashTable->buckets[bucketIndex], hashValue);
}

// Returns the number of hashed values in the hash table
int sizeOfHashTable(HashTable *hashTable) {
	if (hashTable == NULL) {
		ERROR_PRINT("Hash Table uninitialized");
		return 1;
	}

	int size = 0;
	for (int i = 0; i < hashTable->numBuckets; i++) {
		size += listLength(hashTable->buckets[i]);
	}

	return size;
}
