#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "dbg.h"

// Note that *head MUST point to NULL on an initial call to a function

// If you change this, you should also change linkedList's printList function to match format
typedef int64_t HASHVALUETYPE;

typedef struct Node {
	HASHVALUETYPE hashValue;
	struct Node *next;  // NULL if end of list
} Node;

// Returns 0 if not already present, other if already present (and will give error message)
int add(Node **head, HASHVALUETYPE hashValue);

// Returns 1 if exists, 0 if not
int contains(Node **head, HASHVALUETYPE hashValue);

// Returns the length of the list (0 if none)
int length(Node **head);

// Returns 0 if successfully deletes, other if not (and will give error message)
int delete(Node **head, HASHVALUETYPE hashValue);

// Returns 0 if successful, other if not.  Deletes all entries.
int flush(Node **head);

// Prints the list
int printList(Node **head);

#endif