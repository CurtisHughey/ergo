#include "uct.h"

// I want to use AMAF/RAVE^^^
// I should make it illegal to fill in eyes (when would it ever be good (actually, it's sometimes good))?  And it's messing up endgame evaluation ^^^)

// Creates new root UctNode
UctNode *createRootUctNode(State *state, HashTable *hashTable) {
	UctNode *root = malloc(sizeof(UctNode));
	
	root->action = ROOT_MOVE;
	root->reward = 0;  // I guess?
	root->visitCount = 0;
	root->terminal = 0;  // Should never be terminal itself
	root->parent = NULL;  // No parent
	
	Moves *moves = getMoves(state, hashTable);  // If hashTable is NULL, getMoves will ignore
	setChildren(root, moves, state);
	destroyMoves(moves);

	return root;
}

void displayUctTree(UctNode *node) {
	_displayUctTree(node, 0);
}

void _displayUctTree(UctNode *node, int tabs) {
	char *whiteSpace = calloc(tabs, sizeof(char));
	for (int i = 0; i < tabs; i++) {
		whiteSpace[i] = '\t';
	}
	printf("%s%d\n", whiteSpace, node->action);
	for (int i = 0; i < node->childrenCount; i++) {
		_displayUctTree(node->children[i], tabs+1);
	}
}

// Adds children to UctNode
void expandUctNode(State *state, UctNode *parent, HashTable *hashTable) {
	UnmakeMoveInfo unmakeMoveInfo;

	Moves *moves = NULL;
	if (parent->action != ROOT_MOVE) {
		makeMoveAndSave(state, parent->action, &unmakeMoveInfo, hashTable);
		moves = getMoves(state, hashTable);  // Note that we're only adding children that are superko legal
		unmakeMove(state, &unmakeMoveInfo, hashTable);
	} else {
		moves = getMoves(state, hashTable);
	}

	setChildren(parent, moves, state);	
	destroyMoves(moves);
}

void setChildren(UctNode *parent, Moves *moves, State *state) {  // Switch order of arguments at some point  ^^
	UctNode **children = calloc(moves->count, sizeof(UctNode *));
	for (int i = 0; i < moves->count; i++) {
		children[i] = malloc(sizeof(UctNode));

		children[i]->action = moves->array[i];
		children[i]->reward = 0;
		children[i]->visitCount = 0;
		children[i]->terminal = state->blackPassed && moves->array[i] == MOVE_PASS;
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

// Returns the best move.  Entry point (should make everything else static)
int uctSearch(State *state, Config *config, HashTable *hashTable) {
	int rollouts = config->rollouts;
	int lengthOfGame = config->lengthOfGame;
	int threads = config->threads;

	UctNode *root = createRootUctNode(state, hashTable);
	int rootTurn = state->turn;
	for (int i = 0; i < rollouts; i++) {
		State *copy = copyState(state);

		UctNode *v = NULL;
		if (hashTable != NULL) {
			v = treePolicy(copy, root, hashTable);
		} else {
			v = treePolicyNoHashing(copy, root);
		}

		double reward = defaultPolicy(rootTurn, copy, v, lengthOfGame, threads);
		backupNegamax(v, reward);
		destroyState(copy);
	}

	UctNode *bestNode = bestChild(root, 0);
	int move = bestNode->action;

	destroyUctNode(root);

	return move;
}

UctNode *treePolicy(State *state, UctNode *v, HashTable *hashTable) {
	assert(hashTable != NULL);  // Good check to have, I guess

	HASHVALUETYPE *hashValues = calloc(ESTIMATED_DEPTH, sizeof(HASHVALUETYPE));  // These are the hash values we'll need to delete
	int index = 0;
	int hashValuesLength = ESTIMATED_DEPTH;

	while (!v->terminal) {  // Will stop when it finds a terminal node
		if (v->childrenVisited < v->childrenCount) {  // I.e. not fully expanded
			v = expand(state, v, hashTable);
			makeMove(state, v->action, hashTable);
			hashValues[index++] = zobristHash(state);
			break;
		} else {
			v = bestChild(v, UCT_CONSTANT);
			makeMove(state, v->action, hashTable);
			hashValues[index++] = zobristHash(state);
		}

		if (index >= hashValuesLength) {
			hashValuesLength *= 2;
			hashValues = realloc(hashValues, hashValuesLength * sizeof(HASHVALUETYPE));
		}
	}

	// Now deletes the hash values
	for (int i = 0; i < index; i++) {
		deleteValueFromHashTable(hashTable, hashValues[i]);
	}	
	free(hashValues);
	hashValues = NULL;

	return v;
}


UctNode *treePolicyNoHashing(State *state, UctNode *v) {
	while (!v->terminal) {  // Will stop when it finds a terminal node
		if (v->childrenVisited < v->childrenCount) {  // I.e. not fully expanded
			v = expand(state, v, NULL);
			makeMove(state, v->action, NULL);
			break;
		} else {
			v = bestChild(v, UCT_CONSTANT);
			makeMove(state, v->action, NULL);
		}
	}

	return v;
}

// Chooses a random unexplored child (v')
UctNode *expand(State *state, UctNode *v, HashTable *hashTable) {
	int numUnvisited = v->childrenCount - v->childrenVisited;
	assert (numUnvisited > 0);  // A nice assert to have
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

	expandUctNode(state, child, hashTable);
	v->childrenVisited += 1;  // We just visited a new child

	return child;
}

// Returns the best child by the UCB1 algorithm
UctNode *bestChild(UctNode *v, double c) {
	double bestReward = INT_MIN;
	int bestChildIndex = 0;  // There is always at least 1 child, so this will be filled
	for (int i = 0; i < v->childrenCount; i++) {
		UctNode *child = v->children[i];
		double reward = calcReward(v, child, c);
		if (reward > bestReward) {
			bestReward = reward;
			bestChildIndex = i;
		}

		// if (c == 0) {  // Remove ^^^^
		// 	char *x = moveToString(v->children[i]->action, 1); 
		// 	printf("%s, %lf, %d\n", x, reward, v->children[i]->visitCount);
		// 	free(x);
		// }
	}
	assert(bestChildIndex != -1);

	return v->children[bestChildIndex];
}

double calcReward(UctNode *parent, UctNode *child, double c) {
	double reward = 0;
	if (child->visitCount > 0) {  // I.e. if visited.
		reward = ((double)(child->reward))/child->visitCount 
					+ c*sqrt(2*log((double)parent->visitCount)/child->visitCount); // Might need *2 ^^^
	} else {
		reward = INT_MIN;
	}
	return reward;
}

// Simulates rest of game, for lengthOfGame moves
// state will (probably) be mutated, you should save if you want it later
// For this, we do NOT care about superko ^^^
double defaultPolicy(int rootTurn, State *state, UctNode *v, int lengthOfGame, int threads) {
	assert(threads >= 1);  // Otherwise, this is bad

	double reward = -1;

	if (threads == 1) {  // Then no need to do pthread stuff
		DefaultPolicyWorkerInput *dpwi = malloc(sizeof(DefaultPolicyWorkerInput));
		dpwi->rootTurn = rootTurn;
		dpwi->state = state;  // No need to copy
		dpwi->lengthOfGame = lengthOfGame;
		dpwi->v = v;

		DefaultPolicyWorkerOutput *dpwo = defaultPolicyWorker((void *)dpwi);
		reward = dpwo->reward;

		free(dpwo);
		dpwo = NULL;
		free(dpwi);
		dpwi = NULL;
	}

	return reward;
}

void *defaultPolicyWorker(void *args) {
	DefaultPolicyWorkerInput *dpwi = (DefaultPolicyWorkerInput *)args;
	int rootTurn = dpwi->rootTurn;
	State *state = dpwi->state;
	int lengthOfGame = dpwi->lengthOfGame;
	UctNode *v = dpwi->v;

	int color = state->turn;
	int prevNumMoves = 0;  // Maybe we could guess based on how many moves have happened

	if (!v->terminal) {
		for (int i = 0; i < lengthOfGame; i++) {  // At some point, this is going to have to be dynamic
			int blackPassed = state->blackPassed;

			int randomMove = -2;
			if (prevNumMoves < 5) {  // Magically hardcoded ^^^, need to adjust for board size.  This is forgiveness prediction.  Also handles initial prevNumMoves=0
				Moves *moves = getMoves(state, NULL);  // It sucks that I have to keep calling getMoves, maybe there's a way to speed it up by passing in moves? ^^^
				int randomIndex = rand() % moves->count;
				randomMove = moves->array[randomIndex];
				makeMove(state, randomMove, NULL);
				prevNumMoves = moves->count;
				destroyMoves(moves);
			}
			else {  // Better to do it first and ask forgiveness later.
				int counter = 0;
				do {
					randomMove = rand() % (BOARD_SIZE+1);  // This will need to have a seed once we do parallel
					if (randomMove == BOARD_SIZE) {  // This represented a pass
						randomMove = MOVE_PASS;
					}
					counter += 1;
				} while (!isLegalMove(state, randomMove, NULL));  // Worth giving up at some time? ^^^

				makeMove(state, randomMove, NULL);
				prevNumMoves = BOARD_SIZE/counter;  // Its best guess
				if (prevNumMoves < 0) {
					prevNumMoves = 1;  // You can always pass.  Not really necessary to do this check
				}
			}

			// This means if both players randomly pass in the middle of the game, we stop
			if (randomMove == MOVE_PASS && blackPassed) {  // I.e. if the last move was a pass by white (so the turn just became black), and black had also passed
				break;  // We're done
			}
		}
	}
	
	Score scores = calcScores(state);
	
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
	if (rootTurn == STATE_WHITE) {
		reward = 1-reward;
	}

	if (color == rootTurn) {
		reward = -1*reward;  // To work with negamax
	}

	DefaultPolicyWorkerOutput *dpwo = malloc(sizeof(DefaultPolicyWorkerOutput));
	dpwo->reward = reward;

	return (void *)dpwo;
}

// Propagates new score back to root
void backupNegamax(UctNode *v, double reward) {
	while (v != NULL) {
		v->visitCount += 1;
		v->reward += reward;
		reward = -1*reward;
		v = v->parent;
	}
}
