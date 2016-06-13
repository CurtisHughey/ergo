#include "uct.h"

// Creates new UctNode
UctNode *createUctNode(State *state, int move) {
	UctNode *v = malloc(sizeof(UctNode));
	
	v->action = move;
	v->reward = 0;  // I guess?
	v->visitCount = 0;
	
	UnmakeMoveInfo unmakeMoveInfo;
	makeMoveAndSave(state, move, &unmakeMoveInfo);
	Moves *moves = getMoves(state);  // Maybe should have a second function that returns UctNodes
	unmakeMove(state, &unmakeMoveInfo);

	UctNode **children = calloc(moves->count, sizeof(UctNode *));
	for (int i = 0; i < moves->count; i++) {
		children[i]->action = moves->array[i];
		children[i]->reward = 0;
		children[i]->visitCount = 0;
		children[i]->children = NULL;
		children[i]->childrenCount = 0;
	}
	v->children = children;
	v->childrenCount = moves->count;

	free(moves);

	return v;
}

// Recursively destroys UctNodes
void destroyUctNode(UctNode *v) {
	for (int i = 0; i < v->childrenCount; i++) {
		destroyUctNode(v->children[i]);
	}
	free(v);
	v = NULL;
}

// Returns the best move
// Random right now for testing
int uctSearch(State *state) {
	Moves *moves = getMoves(state);
	srand(time(NULL));
	int randIndex = rand() % moves->count;
	int move = moves->array[randIndex];
	free(moves);
	return move;
}

// // Finds non-terminal node
// UctNode *treePolicy(UctNode *v);

// // Creates new child node
// UctNode *expand(UctNode *v);

// // Returns the best child by the UCB1 algorithm
// UctNode *bestChild(UctNode *v);

// // Simulates rest of game, for lengthOfGame moves
// double defaultPolicy(State *state, int lengthOfGame);

// // Propagates new score back to root
// void backupNegamax(UctNode *v, double reward);

// // Used for the second argument of defaultPolicy.  Just returning constant right now 
// int chooseLengthOfGame(int lengthSoFar);