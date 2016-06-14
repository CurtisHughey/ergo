#include "uct.h"

// Creates new UctNode
UctNode *createUctNode(State *state, UctNode *parent, int move) {
	UctNode *v = malloc(sizeof(UctNode));
	
	v->action = move;
	v->reward = 0;  // I guess?
	v->visitCount = 0;
	
	UnmakeMoveInfo unmakeMoveInfo;
	if (move != ROOT_MOVE) {
		makeMoveAndSave(state, move, &unmakeMoveInfo);
	}
	Moves *moves = getMoves(state);  // Maybe should have a second function that returns UctNodes
	
	if (move != ROOT_MOVE) {
		unmakeMove(state, &unmakeMoveInfo);
	}

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
	v->childrenVisited = 0;
	v->parent = NULL;

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
// TODO
int uctSearch(State *state) {
	// Moves *moves = getMoves(state);
	// srand(time(NULL));
	// int randIndex = rand() % moves->count;
	// int move = moves->array[randIndex];
	// free(moves);
	// return move;

	UctNode *root = createUctNode(state, NULL, -2);
	UNUSED(root);

	return 0;
}

// // Finds non-terminal node
// UctNode *treePolicy(UctNode *v);

// // Creates new child node
// UctNode *expand(UctNode *v);

// Returns the best child by the UCB1 algorithm
UctNode *bestChild(UctNode *v) {
	int bestReward = 0;  // INT_MIN?
	int bestChildIndex = -1;  // There is always at least 1 child, so this will be filled
	for (int i = 0; i < v->childrenCount; i++) {
		UctNode *child = v->children[i];
		int reward = 0;
		if (child->visitCount > 0) {  // I.e. if visited
			reward = child->reward/child->visitCount 
					+ UCT_CONSTANT*sqrt(log((double)v->visitCount)/child->visitCount);
		} else {
			// This is bad because it will favor later children ^^^
			reward = INT_MAX;  // Highest possible
		}
		if (reward > bestReward) {
			bestReward = reward;
			bestChildIndex = i;
		}
	}
	return v->children[bestChildIndex];
}

// Simulates rest of game, for lengthOfGame moves
// state will be unchanged
double defaultPolicy(State *state, int lengthOfGame) {
	int color = state->turn;
	State *playoutCopy = copyState(state);

	srand(time(NULL));  // I should stop doing this all over the place, just initialize in beginning ^^^

	for (int i = 0; i < lengthOfGame; i++) {
		Moves *moves = getMoves(playoutCopy);  // It sucks that I have to keep calling getMoves, maybe there's a way to speed it up by passing in moves? ^^^
		int randomIndex = rand() % moves->count;
		makeMove(playoutCopy, moves->array[randomIndex]);
		free(moves);
		moves = NULL;
	}

	Score scores = calcScores(playoutCopy);
	
	// Calculates reward for black
	double reward = 0;
	if (scores.blackScore > scores.whiteScore) {
		reward = 1;
	} else if (scores.blackScore < scores.whiteScore) {
		reward = 0;
	} else {
		reward = 0.5;
	}
	// Reverses reward if necessary
	if (color == STATE_WHITE) {
		reward = 1-reward;
	}

	destroyState(playoutCopy);

	return reward;
}

// Propagates new score back to root
void backupNegamax(UctNode *v, double reward) {
	while (v != NULL) {
		v->visitCount += 1;
		v->reward += reward;
		reward *= -1;
		v = v->parent;
	}
}

// Used for the second argument of defaultPolicy.  Just returning constant right now 
int chooseLengthOfGame(int lengthSoFar) {
	// Lol, IDK
	if (lengthSoFar < 100) {
		return 200;
	} else if (lengthSoFar < 200) {
		return 100;
	} else {
		return 50;
	}
}