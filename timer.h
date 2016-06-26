// This code was adapted from Hank Hoffman's Parallel class at UChicago

#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <x86_64-linux-gnu/sys/time.h>  // This probably isn't good to have ^^^

typedef struct {
	struct timeval startTime;
	struct timeval stopTime;
} Timer;

void startTimer(Timer *timer);

void stopTimer(Timer *timer);

double timeElapsed(Timer *timer);

#endif