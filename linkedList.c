#include "linkedList.h"

// Returns 0 if not already present, other if already present (and will give error message)
int add(Node **head, HASHVALUETYPE hashValue) {
	if (!head) {
		ERROR_PRINT("Failed to add node");
		return 1;  // Wasn't allocated, this is pretty bad
	}

	// Need to check first for empty list
	if (!(*head)) {
		*head = malloc(sizeof(Node));
		if (*head) {
			(*head)->hashValue = hashValue;
			(*head)->next = NULL;
			return 0;
		}
		else {
			ERROR_PRINT("Failed to allocate head");
			return 1;	
		}
	} 

	// Now just inserts the hashValue at the head position

	Node *newNode = malloc(sizeof(Node));
	if (!newNode) {
		ERROR_PRINT("Failed to allocate");
		return 0;
	}

	newNode->hashValue = hashValue;
	newNode->next = *head;  // Previous head moved down one
	*head = newNode;  // Our newly created node is now the head

	return 0;
}

// Returns 1 if exists, 0 if not
int search(Node **head, HASHVALUETYPE hashValue);

// Returns the length of the list (0 if none)
int length(Node **head);

// Returns 0 if not already present, other if already present (and will give error message)
int delete(Node **head, HASHVALUETYPE hashValue);

// Returns 0 if successful, other if not.  Deletes all entries.
int flush(Node **head) {
	Node *current = *head;

	while (current) {
		Node *temp = current;
		current = current->next;
		//free(temp);
		temp = NULL;
		UNUSED(temp);
	}

	*head = NULL;  // for good measure

	return 0;
}