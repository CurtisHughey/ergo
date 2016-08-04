#include "uct.h"

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
	assert(threads >= 1);  // Making this check again

	int numIterations = rollouts/threads;

	DefaultPolicyWorkerInput **dpwis = NULL;
	pthread_t *workers = NULL;
	// Kicks off the worker threads for simulations
	if (threads > 1) {  // Then need to allocate for workers and their input
		dpwis = calloc(threads, sizeof(DefaultPolicyWorkerInput *));  // Could probably also do this on the stack
		workers = calloc(threads, sizeof(pthread_t));

		for (int i = 0; i < threads; i++) {  
			// Set up input
			dpwis[i] = malloc(sizeof(DefaultPolicyWorkerInput));
			dpwis[i]->tid = i;
			dpwis[i]->workerFinished = 0;  // Obviously hasn't finished an iteration yet
			dpwis[i]->shouldProcess = 0;  // Not ready yet
			dpwis[i]->shouldDie = 0;  // Shouldn't die yet, obviously

			// Kick off the threads
			pthread_create(&workers[i], NULL, defaultPolicyWorker, (void *)dpwis[i]);  // The ith worker is responsible for the ith dpwi
		}	
	}

	UctNode *root = createRootUctNode(state, hashTable);
	int rootTurn = state->turn;
	for (int i = 0; i <= numIterations; i++) {  // <= to make sure we get at least one iteration, in case rollouts<threads (unlikely)
		State *copy = copyState(state);

		UctNode *v = NULL;
		if (hashTable != NULL) {
			v = treePolicy(copy, root, hashTable);
		} else {
			v = treePolicyNoHashing(copy, root);
		}

		double reward = defaultPolicy(rootTurn, copy, v, lengthOfGame, workers, dpwis, threads);
		backupNegamax(v, reward, threads);  // threads needs to get passed so the visit count can be properly updated
		destroyState(copy);
	}

	UctNode *bestNode = bestChild(root, 0);
	int move = bestNode->action;

	// End the threads, if they exist
	if (threads > 1) {
		for (int i = 0; i < threads; i++) {
			dpwis[i]->shouldDie = 1;
		}

		for (int i = 0; i < threads; i++) {
			if (pthread_join(workers[i], NULL)) {
				ERROR_PRINT("Failed to join thread %d correctly", i);
			}
			free(dpwis[i]);
			dpwis[i] = NULL;		
		}

		free(dpwis);
		dpwis = NULL;
		free(workers);
		workers = NULL;
	}

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
double defaultPolicy(int rootTurn, State *state, UctNode *v, int lengthOfGame, pthread_t *workers, DefaultPolicyWorkerInput **dpwis, int threads) {
	assert(threads >= 1);  // Otherwise, this is bad

	double reward = -1;

	if (threads == 1) {  // Then no need to do pthread stuff
		reward = simulate(rootTurn, state, lengthOfGame, v);
	} else {  // Then we need to pass info to the worker threads
		for (int i = 0; i < threads; i++) {
			// Set up input
			dpwis[i]->rootTurn = rootTurn;
			dpwis[i]->state = copyState(state);
			dpwis[i]->lengthOfGame = lengthOfGame;
			dpwis[i]->v = v;

			// Indicate that the threads can process
			dpwis[i]->shouldProcess = 1;
		}

		// Wait for threads to finish processing
		for (int i = 0; i < threads; i++) {
			while (!dpwis[i]->workerFinished) {}  // Spins - is there a synchronize thing going on (is the compiler going to eliminate this? ^^^)
			dpwis[i]->workerFinished = 0;  // For next time

			reward += dpwis[i]->reward;

			destroyState(dpwis[i]->state);  // This was a copy
		}	
	}

	return reward;
}

double simulate(int rootTurn, State *state, int lengthOfGame, UctNode *v) {
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

	return reward;	
}

void *defaultPolicyWorker(void *args) {
	DefaultPolicyWorkerInput *dpwi = (DefaultPolicyWorkerInput *)args;
	while (!dpwi->shouldDie) {
		if (dpwi->shouldProcess) {
			int rootTurn = dpwi->rootTurn;
			State *state = dpwi->state;
			int lengthOfGame = dpwi->lengthOfGame;
			UctNode *v = dpwi->v;

			dpwi->reward = simulate(rootTurn, state, lengthOfGame, v);

			dpwi->shouldProcess = 0;  // Must wait for the caller to set this
			dpwi->workerFinished = 1;  // Worker finished, caller can now give another dpwi
		} else {
			sched_yield();  // Might as well
		}
	}

	return NULL;  // Maybe could return stats?
}

// Propagates new score back to root
void backupNegamax(UctNode *v, double reward, int threads) {
	while (v != NULL) {
		v->visitCount += threads;
		v->reward += reward;
		reward = -1*reward;
		v = v->parent;
	}
}
