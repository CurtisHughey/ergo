#include "uct.h"

// Creates new root UctNode
UctNode *createRootUctNode(State *state, HashTable *hashTable) {
	UctNode *root = malloc(sizeof(UctNode));
	
	root->action = ROOT_MOVE;
	root->reward = 0;  // I guess?
	root->visitCount = 0;
	root->terminal = 0;  // Should never be terminal itself
	root->parent = NULL;  // No parent

	root->amafVisits = 0;
	root->amafReward = 0;
	
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

		children[i]->amafVisits = 0;
		children[i]->amafReward = 0;
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
	int respect = config->respect;
	int raveV = config->raveV;

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

	// The meat of the algorithm, for each rollout finds the best node to explore, simulates the game, and then propagates it back to the root
	UctNode *root = createRootUctNode(state, hashTable);
	int rootTurn = state->turn;
	for (int i = 0; i <= numIterations; i++) {  // <= to make sure we get at least one iteration, in case rollouts<threads (unlikely)
		State *copy = copyState(state);

		UctNode *v = NULL;
		if (hashTable != NULL) {
			v = treePolicy(copy, root, hashTable, raveV);
		} else {
			v = treePolicyNoHashing(copy, root, raveV);
		}

		defaultPolicyAndBackup(rootTurn, copy, v, lengthOfGame, workers, dpwis, threads, raveV);

		destroyState(copy);
	}

	// Finally extracts the best move
	UctNode *bestNode = bestChild(root, 0, respect, raveV, 1);  // 1 because we intend to make the move on the user end
	int move = bestNode == NULL ? MOVE_RESIGN : bestNode->action;  // Resigns if bestNode is NULL

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

UctNode *treePolicy(State *state, UctNode *v, HashTable *hashTable, int raveV) {
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
			v = bestChild(v, UCT_CONSTANT, -1, raveV, 0);  // -1 for never resigning, 0 because we don't intend to return this to user
			makeMove(state, v->action, hashTable);
			hashValues[index++] = zobristHash(state);
		}

		if (index >= hashValuesLength) {  // Then need to expand
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


UctNode *treePolicyNoHashing(State *state, UctNode *v, int raveV) {
	while (!v->terminal) {  // Will stop when it finds a terminal node
		if (v->childrenVisited < v->childrenCount) {  // I.e. not fully expanded
			v = expand(state, v, NULL);
			makeMove(state, v->action, NULL);
			break;
		} else {
			v = bestChild(v, UCT_CONSTANT, -1, raveV, 0);  // -1 for never resigning
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
UctNode *bestChild(UctNode *v, double c, int respect, int raveV, int atRoot) {
	double bestReward = INT_MIN;
	int bestChildIndex = 0;  // There is always at least 1 child, so this will be filled
	for (int i = 0; i < v->childrenCount; i++) {
		UctNode *child = v->children[i];
		double reward = calcReward(v, child, c, raveV);
		if (reward > bestReward) {
			bestReward = reward;
			bestChildIndex = i;
		}

		// if (c == 0) {  // Remove ^^^
		// 	char *x = moveToString(v->children[i]->action, 1); 
		// 	printf("%s, %lf, %d\n", x, reward, v->children[i]->visitCount);
		// 	free(x);
		// }
	}

	if (atRoot && bestReward*100 < (double)respect) {  // Then resign
		return NULL;  // NULL means resignation
	} else {
		return v->children[bestChildIndex];
	}
}

double calcReward(UctNode *parent, UctNode *child, double c, int raveV) {
	double totalReward = 0;

	// First calculates uctReward
	double uctScore = 0;
	if (child->visitCount > 0) {  // I.e. if visited.
		uctScore = (double)child->reward/child->visitCount 
					+ c*sqrt(log((double)parent->visitCount)/child->visitCount); // Might need *2 ^^^
	} else {
		uctScore = INT_MIN;  // This is arguably bad (maybe we need to require that the vistcount is greater than 0)
	}

	if (raveV != 0) {  // Then need to calculate AMAF/RAVE reward
		double alpha = ((double)(raveV - child->visitCount)) / raveV;  // I believe this is correct or should it be child->amafVists? ^^^
		if (alpha < 0) {
			alpha = 0;
		}

		double amafScore = child->amafReward/child->amafVisits;

		totalReward = alpha*amafScore + (1-alpha)*uctScore;
	} else {
		totalReward = uctScore;  // No AMAF/RAVE
	}

	return totalReward;
}

// Simulates rest of game, for lengthOfGame moves
// state will (probably) be mutated, you should save if you want it later
// For this, we do NOT care about superko ^^^
void defaultPolicyAndBackup(int rootTurn, State *state, UctNode *v, int lengthOfGame, pthread_t *workers, DefaultPolicyWorkerInput **dpwis, int threads, int raveV) {
	assert(threads >= 1);  // Otherwise, this is bad

	if (threads == 1) {  // Then no need to do pthread stuff
		RewardData *rewardData = simulate(rootTurn, state, lengthOfGame, v, raveV);
		rewardBackup(v, rewardData);  // Backs up on all the moves in rewardData->moves (NULL if no AMAF)
		destroyRewardData(rewardData);  // rewardData->moves = NULL if no RAVE
	} else {  // Then we need to pass info to the worker threads
		for (int i = 0; i < threads; i++) {
			// Set up input
			dpwis[i]->rootTurn = rootTurn;
			dpwis[i]->state = copyState(state);
			dpwis[i]->lengthOfGame = lengthOfGame;
			dpwis[i]->v = v;
			dpwis[i]->raveV = raveV;

			// Indicate that the threads can process
			dpwis[i]->shouldProcess = 1;
		}

		// Wait for threads to finish processing
		for (int i = 0; i < threads; i++) {
			while (!dpwis[i]->workerFinished) {  // Spins - is there a synchronize thing going on (is the compiler going to eliminate this? ^^^)
				usleep(1);  // I think this helps, lol
				sched_yield();
			}
			dpwis[i]->workerFinished = 0;  // For next time

			RewardData *workerRewardData = dpwis[i]->rewardData;  // The data that the worker produced

			rewardBackup(v, workerRewardData);  // Will call backupNegamax on v and all the distinct moves in workerRewardData

			destroyRewardData(workerRewardData);
			destroyState(dpwis[i]->state);  // This was a copy
		}	
	}
}


// Maybe rename to rollout ^^^
RewardData *simulate(int rootTurn, State *state, int lengthOfGame, UctNode *v, int raveV) {
	int color = state->turn;

	RewardData *rewardData = createRewardData();
	Moves *moves = rewardData->moves;  // Just to save some typing later

	if (!v->terminal) {
		for (int i = 0; i < lengthOfGame; i++) {  // At some point, this is going to have to be dynamic
			int blackPassed = state->blackPassed;

			int randomMove = -3;  // Initializing to something bad
			// Now keeps guessing until finds legal move.  Might be bad...
			do {
				randomMove = rand() % (BOARD_SIZE+1);  // This will need to have a seed once we do parallel
				if (randomMove == BOARD_SIZE) {  // This represented a pass
					randomMove = MOVE_PASS;
				}
			} while (!isLegalMove(state, randomMove, NULL));  // Worth giving up at some time? ^^^

			makeMove(state, randomMove, NULL);

			if (raveV != 0) {  // Means we're doing RAVE
				moves->array[moves->count++] = randomMove;  // Adds it
				if (moves->count >= moves->size) {
					moves->size *= 2;
					moves->array = realloc(moves->array, moves->size*sizeof(int));
				}
			}

			// This means if both players randomly pass in the middle of the game, we stop
			if (randomMove == MOVE_PASS && blackPassed) {  // I.e. if the last move was a pass by white (so the turn just became black), and black had also passed
				break;  // We're done
			}
		}
	}

	rewardData->moves = moves;  // Need to reassign because of potential reallocs
	
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
	// Reverses reward if white
	if (rootTurn == STATE_WHITE) {
		reward = 1-reward;
	}

	if (color == rootTurn) {
		reward = -1*reward;  // To work with negamax, needs to reverse if there would be an uneven amount of *-1 applied in the backup function
	}

	rewardData->reward = reward;

	return rewardData;	
}

void *defaultPolicyWorker(void *args) {
	DefaultPolicyWorkerInput *dpwi = (DefaultPolicyWorkerInput *)args;
	while (!dpwi->shouldDie) {
		if (dpwi->shouldProcess) {
			int rootTurn = dpwi->rootTurn;
			State *state = dpwi->state;
			int lengthOfGame = dpwi->lengthOfGame;
			UctNode *v = dpwi->v;
			int raveV = dpwi->raveV;

			dpwi->rewardData = simulate(rootTurn, state, lengthOfGame, v, raveV);

			dpwi->shouldProcess = 0;  // Must wait for the caller to set this
			dpwi->workerFinished = 1;  // Worker finished, caller can now give another dpwi
		} else {
			sched_yield();  // Might as well
		}
	}

	return NULL;  // Maybe could return stats?
}

// Propagates new UCT score back to root
void backupNegamaxUCT(UctNode *v, double reward, int threads) {
	while (v != NULL) {
		v->visitCount += threads;
		v->reward += reward;
		reward = -1*reward;
		v = v->parent;
	}
}

// Propagates new AMAF score back to root
void backupNegamaxAMAF(UctNode *v, double reward, int threads) {
	while (v != NULL) {
		v->amafVisits += threads;
		v->amafReward += reward;
		reward = -1*reward;
		v = v->parent;
	}
}

// Backs up, single thread, all the moves in amafData
void rewardBackup(UctNode *v, RewardData *rewardData) {
	if (rewardData->moves != NULL) {  // Doing RAVE!  Now needs to search through UCT nodes and update them appropriately
		UctNode *parent = v->parent;  // Needs to look through all the parent's children

		// Note that we ignore MOVE_PASS
		int pointRecorded[BOARD_SIZE] = {0};  // Makes sure we don't back propagate multiple times

		if (v->action != MOVE_PASS) {
			pointRecorded[v->action] = 1;  // This is taken care of at the end of the function
		}

		for (int i = 0; i < rewardData->moves->count; i++) {
			int move = rewardData->moves->array[i];
			if (move != MOVE_PASS && !pointRecorded[move]) {  // Then not recorded, and need to find UCT node
				// Actually, could probably do a binary search? ^^^
				for (int j = 0; j < parent->childrenCount; j++) {
					UctNode *uctNode = parent->children[j];
					if (move == uctNode->action) {  // Then we found our UCT node!
						// Need to update AMAF values
						backupNegamaxAMAF(uctNode, rewardData->reward, 1);  // Assuming only one thread
						break;
					}
				}  // Maybe should make sure we found it? ^^^
				pointRecorded[move] = 1;  // Ignore if we see it another time
			}
		}
	}

	backupNegamaxUCT(v, rewardData->reward, 1);  // Update uct reward values
}

RewardData *createRewardData(void) {
	RewardData *rewardData = malloc(sizeof(RewardData));
	rewardData->moves = createMoves();
	rewardData->reward = 0;  // Nothing yet

	return rewardData;
}

void destroyRewardData(RewardData *rewardData) {
	if (rewardData->moves != NULL) {  // NULL if not using RAVE/AMAF
		destroyMoves(rewardData->moves);
	}

	free(rewardData);
	rewardData = NULL;
}
