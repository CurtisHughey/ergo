#ifndef _HASH_H
#define _HASH_H

#include <stdio.h>
#include <stdlib.h>

#include "state.h"
#include "linkedList.h"
#include "dbg.h"

#define RANDOM_NUMBER_FILE "zobristData/randomNumbers.txt"

// The random numbers
HASHVALUETYPE blackVals[BOARD_SIZE];
HASHVALUETYPE whiteVals[BOARD_SIZE];
HASHVALUETYPE emptyVals[BOARD_SIZE];
HASHVALUETYPE koVal;
////

typedef struct {
	Node ***buckets;
	int numBuckets;
} HashTable;

// Reads in the random numbers from the file, and then writes them into the global arrays above
void initHashVals(void);

// Calculates the Zobrist hash of a given position
HASHVALUETYPE zobristHash(State *state);

// Creates a hash table with numBuckets buckets (linked lists)
HashTable *createHashTable(int numBuckets);

// Destroys the hash table
void destroyHashTable(HashTable *hashTable);

// Adds a state to the hash table.  Returns 0 upon success
int addToHashTable(HashTable *hashTable, State *state);

// Deletes a state to the hash table.  Returns 0 upon success
int deleteFromHashTable(HashTable *hashTable, State *state);

// Sees if a state is contained in the hash table.  Returns 1 if true, 0 if not
int containsInHashTable(HashTable *hashTable, State *state);

// Returns the number of hashed values in the hash table
int sizeOfHashTable(HashTable *hashTable);

#endif