// This code was adapted from Hank Hoffman's Parallel class at UChicago

#ifndef TIMER_H
#define TIMER_H

#include <stdlib.h>
#include <sys/time.h>

typedef struct {
	struct timeval startTime;
	struct timeval stopTime;
} Timer;

void startTimer(Timer *timer);

void stopTimer(Timer *timer);

double timeElapsed(Timer *timer);

#endif