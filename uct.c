#include "uct.h"

// I want to use AMAF/RAVE^^^

// Creates new root UctNode
UctNode *createRootUctNode(State *state) {
	UctNode *root = malloc(sizeof(UctNode));
	
	root->action = ROOT_MOVE;
	root->reward = 0;  // I guess?
	root->visitCount = 0;
	root->parent = NULL;  // No parent
	
	Moves *moves = getMoves(state);
	setChildren(root, moves);
	free(moves);

	return root;
}

// Adds children to UctNode
void expandUctNode(State *state, UctNode *parent) {
	UnmakeMoveInfo unmakeMoveInfo;

	State *copy = copyState(state);

	makeMoveAndSave(state, parent->action, &unmakeMoveInfo);
	Moves *moves = getMoves(state);  // Maybe should have a second function that returns UctNodes
	unmakeMove(state, &unmakeMoveInfo);

	assert(statesAreEqual(copy, state));

	setChildren(parent, moves);
	free(moves);
}

void setChildren(UctNode *parent, Moves *moves) {
	UctNode **children = calloc(moves->count, sizeof(UctNode *));
	for (int i = 0; i < moves->count; i++) {
		children[i] = malloc(sizeof(UctNode));

		children[i]->action = moves->array[i];
		children[i]->reward = 0;
		children[i]->visitCount = 0;
		children[i]->children = NULL;
		children[i]->childrenCount = 0;
		children[i]->childrenVisited = 0;
		children[i]->parent = parent;
	}
	parent->children = children;
	parent->childrenCount = moves->count;
	parent->childrenVisited = 0;
}

// Recursively destroys UctNodes
void destroyUctNode(UctNode *v) {
	for (int i = 0; i < v->childrenCount; i++) {
		destroyUctNode(v->children[i]);
	}
	free(v->children);
	v->children = NULL;
	free(v);
	v = NULL;
}

// Returns the best move
int uctSearch(State *state, int iterations) {
	const int lengthOfGame = 250;  // IRDK ^^^

	UctNode *root = createRootUctNode(state);
	
	UctNode *v = NULL;
	for (int i = 0; i < iterations; i++) {
		v = treePolicy(state, root, lengthOfGame);
		double reward = defaultPolicy(state, lengthOfGame);
		backupNegamax(v, reward);
	}

	UctNode *bestNode = bestChild(root);
	int move = bestNode->action;

	destroyUctNode(root);

	return move;
}

// Finds non-terminal node
UctNode *treePolicy(State *state, UctNode *v, int lengthOfGame) {
	for (int i = 0; i < lengthOfGame; i++) {
		if (v->childrenVisited < v->childrenCount) {  // I.e. not fully expanded
			return expand(state, v);
		} else {
			v = bestChild(v);
		}
	}
	return v;
}

// Chooses a random unexplored child (v')
UctNode *expand(State *state, UctNode *v) {
	srand(time(NULL));  // I should stop doing this all over the place, just initialize in beginning ^^^

	int numUnvisited = v->childrenCount - v->childrenVisited;
	int untriedIndex = rand() % numUnvisited;

	UctNode *child = NULL;
	int searchIndex = 0;
	for (int i = 0; i < v->childrenCount; i++) {
		if (v->children[i]->visitCount == 0) {
			if (searchIndex == untriedIndex) {
				child = v->children[i];
				break;
			} else {
				searchIndex += 1;
			}
		}
	}

	assert(child);

	expandUctNode(state, child);
	v->childrenVisited += 1;  // We just visited a new child

	return child;
}

// Returns the best child by the UCB1 algorithm
UctNode *bestChild(UctNode *v) {
	int bestReward = -1;  // INT_MIN?
	int bestChildIndex = -1;  // There is always at least 1 child, so this will be filled
	for (int i = 0; i < v->childrenCount; i++) {
		UctNode *child = v->children[i];
		int reward = 0;
		if (child->visitCount > 0) {  // I.e. if visited
			reward = child->reward / child->visitCount 
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
	assert(bestChildIndex != -1);

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

		// Use commented policy for beginning of game
		// int randomIndex = -2;
		// do {
		// 	randomIndex = rand() % (BOARD_SIZE+1);
		// } while (!isLegalMove(playoutCopy, randomIndex));

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
	// Lol, IDK ^^^
	if (lengthSoFar < 100) {
		return 200;
	} else if (lengthSoFar < 200) {
		return 100;
	} else {
		return 50;
	}
}