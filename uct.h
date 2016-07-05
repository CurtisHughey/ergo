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
#include "parser.h"

#define ROOT_MOVE -2  // The move that the rootmove has

#define UCT_CONSTANT 0.70710678118 // This is the C_p=1/sqrt(2) term

typedef struct UctNode {
	int action;  // The move
	double reward;  // Maybe just float...
	int visitCount;
	struct UctNode **children;
	int childrenCount;
	int childrenVisited;
	struct UctNode *parent;
} UctNode;

UctNode *createRootUctNode(State *state);

// Adds children to UctNode
void expandUctNode(State *state, UctNode *parent);

// Wrapper function for _displayUctTree
void displayUctTree(UctNode *node);

// Internal, displays the Uct tree with node as the starting root.
void _displayUctTree(UctNode *node, int tabs);

// Explicitly sets children
void setChildren(UctNode *parent, Moves *moves);

// Recursively destroys UctNodes
void destroyUctNode(UctNode *v);

// Returns the best move
int uctSearch(State *state, int iterations);

// Finds non-terminal node
UctNode *treePolicy(State *state, UctNode *v, int lengthOfGame);

// Creates new child node
UctNode *expand(State *state, UctNode *v);

// Returns the best child by the UCB1 algorithm.  c is the constant defined in the paper (either C_p or 0)
UctNode *bestChild(UctNode *v, double c);

// Simulates rest of game, for lengthOfGame moves
double defaultPolicy(State *state, int lengthOfGame);

// Propagates new score back to root
void backupNegamax(UctNode *v, double reward);

// Used for the second argument of defaultPolicy.  Just returning constant right now 
int chooseLengthOfGame(int lengthSoFar);

#endif