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

#define ROOT_MOVE -2  // The move that the rootmove has

#define UCT_CONSTANT 1.41 // This is the C_p=sqrt(2) term

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
int uctSearch(State *state, int rollouts, int lengthOfGame, HashTable *hashTable);

// Finds non-terminal node
UctNode *treePolicy(State *state, UctNode *v, HashTable *hashTable);

// Creates new child node
UctNode *expand(State *state, UctNode *v, HashTable *hashTable);

// Returns the best child by the UCB1 algorithm.  c is the constant defined in the paper (either C_p or 0)
UctNode *bestChild(UctNode *v, double c);

// Calculates the reward by the UCT algorithm
double calcReward(UctNode *parent, UctNode *child, double c);

// Simulates rest of game, for lengthOfGame moves
double defaultPolicy(int rootTurn, State *state, int lengthOfGame, UctNode *v);

// Propagates new score back to root
void backupNegamax(UctNode *v, double reward);

#endif