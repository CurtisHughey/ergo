// Based off http://www.cameronius.com/cv/mcts-survey-master.pdf

#ifndef _UCT_H
#define _UCT_H

#include "state.h"

#define UCT_CONSTANT 2  // This is the 2C_P\sqrt{2} term

typedef struct UctNode {
	int action;  // The move
	double reward;  // Maybe just float...
	int visitCount;
	struct UctNode **children;
	int childrenCount;
} UctNode;

// Creates new UctNode
UctNode *createUctNode(State *state, int move);

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