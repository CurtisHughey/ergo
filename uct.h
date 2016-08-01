// Based off http://www.cameronius.com/cv/mcts-survey-master.pdf

#ifndef _UCT_H
#define _UCT_H

#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <assert.h>

#include "dbg.h"
#include "state.h"
#include "gameParser.h"
#include "hash.h"
#include "configParser.h"

#define ROOT_MOVE -2  // The move that the rootmove has

#define UCT_CONSTANT 0.707106781 // This is the C_p=1/sqrt(2) term

#define ESTIMATED_DEPTH 16

typedef struct UctNode {
	int action;  // The move
	double reward;  // Maybe just float...
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
} DefaultPolicyWorkerInput;

typedef struct {
	double reward;  // Reward (0, 0.5, 1)
} DefaultPolicyWorkerOutput;

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
UctNode *bestChild(UctNode *v, double c);

// Calculates the reward by the UCT algorithm
double calcReward(UctNode *parent, UctNode *child, double c);

// Simulates rest of game, for lengthOfGame moves
// numThreads specifies number of worker threads.  If 1, then it won't make another thread
double defaultPolicy(int rootTurn, State *state, UctNode *v, int lengthOfGame, int threads);

// Single worker for defaultPolicy.  args will be cast to DefaulPolicyWorkerInput
// It is the responsibility of the caller to free args
void *defaultPolicyWorker(void *args);

// Propagates new score back to root
void backupNegamax(UctNode *v, double reward);

#endif