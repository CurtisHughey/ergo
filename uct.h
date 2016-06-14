// Based off http://www.cameronius.com/cv/mcts-survey-master.pdf

#ifndef _UCT_H
#define _UCT_H

#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#include "state.h"

#define ROOT_MOVE -2  // The move that the rootmove has

#define UCT_CONSTANT 2  // This is the 2C_P\sqrt{2} term

typedef struct UctNode {
	int action;  // The move
	double reward;  // Maybe just float...
	int visitCount;
	struct UctNode **children;
	int childrenCount;
	int childrenVisited;
	struct UctNode *parent;
} UctNode;

// Creates new UctNode
UctNode *createUctNode(State *state, UctNode *parent, int move);

// Recursively destroys UctNodes
void destroyUctNode(UctNode *v);

// Returns the best move
int uctSearch(State *state);

// Finds non-terminal node
UctNode *treePolicy(UctNode *v);

// Creates new child node
UctNode *expand(UctNode *v);

// Returns the best child by the UCB1 algorithm
UctNode *bestChild(UctNode *v);

// Simulates rest of game, for lengthOfGame moves
double defaultPolicy(State *state, int lengthOfGame);

// Propagates new score back to root
void backupNegamax(UctNode *v, double reward);

// Used for the second argument of defaultPolicy.  Just returning constant right now 
int chooseLengthOfGame(int lengthSoFar);

#endif