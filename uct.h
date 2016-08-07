// Based off http://www.cameronius.com/cv/mcts-survey-master.pdf

#ifndef _UCT_H
#define _UCT_H

#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>  // Will eventually use this to time the threads for logging
#include <sched.h>

#include "dbg.h"
#include "state.h"
#include "gameParser.h"
#include "hash.h"
#include "configParser.h"

#define ROOT_MOVE -2  // The move that the rootmove has

#define UCT_CONSTANT 1.41421356237 // This is the C_p=sqrt(2) term

#define ESTIMATED_DEPTH 16

typedef struct UctNode {
	int action;  // The move
	double reward;  // Maybe just float...  This is sort of a misnomer, should be like expected value
	int visitCount;
	int terminal;  // Whether it's terminal or not (i.e. it's a white passing move, black passed last turn)
	struct UctNode **children;
	int childrenCount;
	int childrenVisited;
	struct UctNode *parent;
} UctNode;

typedef struct {
	int rootTurn;  // Whose turn it is at the root node
	State *state;  // Current state
	int lengthOfGame;  // Max number of iterations in case we don't have a double pass
	UctNode *v;  // Current UCT node;
	int tid;  // Unique identifier, for debugging
	volatile int shouldProcess;  // If non-zero, means the worker thread can work on it
	volatile int workerFinished;  // Means that the worker has set the reward and is finished
	double reward;  // This is what gets output by the worker thread.
	volatile int shouldDie;  // Non zero if the worker thread should kill itself
} DefaultPolicyWorkerInput;

UctNode *createRootUctNode(State *state, HashTable *hashTable);

// Adds children to UctNode
void expandUctNode(State *state, UctNode *parent, HashTable *hashTable);

// Wrapper function for _displayUctTree
void displayUctTree(UctNode *node);

// Internal, displays the Uct tree with node as the starting root.
void _displayUctTree(UctNode *node, int tabs);

// Explicitly sets children
void setChildren(UctNode *parent, Moves *moves, State *state);

// Recursively destroys UctNodes
void destroyUctNode(UctNode *v);

// Returns the best move
int uctSearch(State *state, Config *config, HashTable *hashTable);

// Finds non-terminal node, with hashing
UctNode *treePolicy(State *state, UctNode *v, HashTable *hashTable);

// Finds non-terminal node, no hashing
UctNode *treePolicyNoHashing(State *state, UctNode *v);

// Creates new child node
UctNode *expand(State *state, UctNode *v, HashTable *hashTable);

// Returns the best child by the UCB1 algorithm.  c is the constant defined in the paper (either C_p or 0)
// if respect == -1, resign will never be returned.  If atRoot == 1 (meaning we intend to return a move to the user) the reward*100 <= respect (0<=reward<=1.0 by definition), then NULL will be returned, meaning it should resign
UctNode *bestChild(UctNode *v, double c, int respect, int atRoot);

// Calculates the reward by the UCT algorithm
double calcReward(UctNode *parent, UctNode *child, double c);

// Simulates rest of game, for lengthOfGame moves
// numThreads specifies number of worker threads.  If 1, then it won't make another thread
double defaultPolicy(int rootTurn, State *state, UctNode *v, int lengthOfGame, pthread_t *workers, DefaultPolicyWorkerInput **dpwis, int threads);

// Simulates the rest of game (the original defaultPolicy in the paper.  Either called serially or in parallel)
double simulate(int rootTurn, State *state, int lengthOfGame, UctNode *v);

// Single worker for defaultPolicy.  args will be cast to DefaulPolicyWorkerInput
// It is the responsibility of the caller to free args
void *defaultPolicyWorker(void *args);

// Propagates new score back to root
void backupNegamax(UctNode *v, double reward, int threads);

#endif