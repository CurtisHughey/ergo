#include "linkedList.h"

// Returns 0 if successful, other if not
int listAdd(Node **head, HASHVALUETYPE hashValue) {
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
int listContains(Node **head, HASHVALUETYPE hashValue) {
	Node *current = *head;
	while (current) {
		if (current->hashValue == hashValue) {
			return 1;
		} else {  // Otherwise, keep going
			current = current->next;
		}
	}

	// If we made it to here, no matches
	return 0;
}

// Returns the length of the list (0 if none)
int listLength(Node **head) {
	int length = 0;

	Node *current = *head;
	while (current) {
		current = current->next;
		length++;
	}	

	return length;
}

// Returns 0 if successfully deletes, other if not (and will give error message)
// Deletes the first occurence
int listDelete(Node **head, HASHVALUETYPE hashValue) {
	if (!head) {  // This would be pretty bad
		DEBUG_PRINT("Failed to delete node");
		return 1;
	}

	if (!(*head)) {  // Then empty
		DEBUG_PRINT("Trying to delete from empty list");
		return 1;
	}

	// Now check if at start of list
	if ((*head)->hashValue == hashValue) {
		Node *temp = (*head)->next;
		free(*head);
		*head = temp;
		return 0;
	}

	// Now just a regular traversal, but we also need to remember the previous
	Node *prev = *head;
	Node *current = prev->next;

	while (current) {
		if (current->hashValue == hashValue) {
			Node *temp = current->next;
			free(current);
			current = NULL;
			prev->next = temp;
			return 0;
		} else {  // Otherwise, keep going
			prev = current;
			current = current->next;
		}
	}

	// If we made it to here, no matches, we failed
	return 1;
}

// Returns 0 if successful, other if not.  Deletes all entries.
int listClear(Node **head) {
	Node *current = *head;

	while (current) {
		Node *temp = current;
		current = current->next;
		free(temp);
		temp = NULL;
	}

	*head = NULL;  // for good measure

	return 0;
}

// Prints the list
int listPrint(Node **head) {
	Node *current = *head;
	
	printf("[");
	while (current) {
		printf("%" PRIu64 ",", current->hashValue);
		current = current->next;
	}	
	printf("]\n");

	return 0;
}