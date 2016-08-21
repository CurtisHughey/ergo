#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "dbg.h"

// If you change this, you should also change linkedList.c's listPrint function to match format, and strtoull in hash.c's initHashVals function
typedef uint64_t HASHVALUETYPE;  // Wow, this really needed to be uint64_t, rather than int64_t (was messing with modulus op stuff)

// Note that *head MUST point to NULL on an initial call to a function

typedef struct Node {
	HASHVALUETYPE hashValue;
	struct Node *next;  // NULL if end of list
} Node;

// Returns 0 if not already present, other if already present (and will give error message)
int listAdd(Node **head, HASHVALUETYPE hashValue);

// Returns 1 if exists, 0 if not
int listContains(Node **head, HASHVALUETYPE hashValue);

// Returns the length of the list (0 if none)
int listLength(Node **head);

// Returns 0 if successfully deletes, other if not (and will give error message)
int listDelete(Node **head, HASHVALUETYPE hashValue);

// Returns 0 if successful, other if not.  Deletes all entries.
int listClear(Node **head);

// Prints the list
int listPrint(Node **head);

#endif