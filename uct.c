#include "uct.h"

// I want to use AMAF/RAVE^^^
// I should make it illegal to fill in eyes (when would it ever be good?  And it's messing up endgame evaluation ^^^)

// Creates new root UctNode
UctNode *createRootUctNode(State *state) {
	UctNode *root = malloc(sizeof(UctNode));
	
	root->action = ROOT_MOVE;
	root->reward = 0;  // I guess?
	root->visitCount = 0;
	root->terminal = 0;  // Should never be terminal itself
	root->parent = NULL;  // No parent
	
	Moves *moves = getMoves(state);
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
void expandUctNode(State *state, UctNode *parent) {
	UnmakeMoveInfo unmakeMoveInfo;

	Moves *moves = NULL;
	if (parent->action != ROOT_MOVE) {
		makeMoveAndSave(state, parent->action, &unmakeMoveInfo);
		moves = getMoves(state);  // Maybe should have a second function that returns UctNodes
		unmakeMove(state, &unmakeMoveInfo);
	} else {
		moves = getMoves(state);
	}

	setChildren(parent, moves, state);	
	destroyMoves(moves);
}

void setChildren(UctNode *parent, Moves *moves, State *state) {
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

// Returns the best move
int uctSearch(State *state, int rollouts, int lengthOfGame) {
	UctNode *root = createRootUctNode(state);
	int rootTurn = state->turn;
	for (int i = 0; i < rollouts; i++) {
		State *copy = copyState(state);
		UctNode *v = treePolicy(copy, root, lengthOfGame);
		double reward = defaultPolicy(rootTurn, copy, lengthOfGame, v);
		backupNegamax(v, reward);
		destroyState(copy);
	}

	UctNode *bestNode = bestChild(root, 0);
	int move = bestNode->action;

	destroyUctNode(root);

	return move;
}

UctNode *treePolicy(State *state, UctNode *v, int lengthOfGame) {
	while (!v->terminal) {  // Will stop when it finds a terminal node
		if (v->childrenVisited < v->childrenCount) {  // I.e. not fully expanded
			UctNode *added = expand(state, v);
			makeMove(state, added->action);
			return added;
		} else {
			v = bestChild(v, UCT_CONSTANT);
			makeMove(state, v->action);
		}
	}
	return v;
}

// Chooses a random unexplored child (v')
UctNode *expand(State *state, UctNode *v) {
	int numUnvisited = v->childrenCount - v->childrenVisited;
	assert (numUnvisited > 0);
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

		// if (c == 0) {  // REmove ^^^^
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
					+ c*sqrt(log((double)parent->visitCount)/child->visitCount); // Might need *2 ^^^
	} else {
		reward = INT_MIN;
	}
	return reward;
}

// Simulates rest of game, for lengthOfGame moves
// state will be mutated, you should save if you want it later
double defaultPolicy(int rootTurn, State *state, int lengthOfGame, UctNode *v) {
	int color = state->turn;

	int prevNumMoves = 0;  // Maybe we could guess based on how many moves have happened

	if (!v->terminal) {
		for (int i = 0; i < lengthOfGame; i++) {  // At some point, this is going to have to be dynamic
			int blackPassed = state->blackPassed;

			int randomMove = -2;
			if (prevNumMoves < 5) {  // Magically hardcoded ^^^, need to adjust for board size.  This is forgiveness prediction.  Also handles initial prevNumMoves=0
				Moves *moves = getMoves(state);  // It sucks that I have to keep calling getMoves, maybe there's a way to speed it up by passing in moves? ^^^
				int randomIndex = rand() % moves->count;
				randomMove = moves->array[randomIndex];
				makeMove(state, randomMove);
				prevNumMoves = moves->count;
				destroyMoves(moves);
			}
			else {  // Better to do it first and ask forgiveness later.
				int counter = 0;
				do {
					randomMove = rand() % (BOARD_SIZE+1);
					if (randomMove == BOARD_SIZE) {  // This represented a pass
						randomMove = MOVE_PASS;
					}
					counter += 1;
				} while (!isLegalMove(state, randomMove));  // Worth giving up at some time? ^^^

				makeMove(state, randomMove);
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

	return reward;
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
