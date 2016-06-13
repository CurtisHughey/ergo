// Based off http://www.cameronius.com/cv/mcts-survey-master.pdf

#ifndef _UCT_H
#define _UCT_H

#include "state.h"

#define UCT_CONSTANT 2  // This is the 2C_P\sqrt{2} term

typedef struct {
	int action;
	double reward;  // Maybe just float...
	int visitCount;
} UctNode;

void uctSearch(State *state);

UctNode treePolicy(UctNode v);

UctNode expand(UctNode v);

UctNode bestChild(UctNode v);

// One more

#endif