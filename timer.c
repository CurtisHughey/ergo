#include "timer.h"

void startTimer(Timer *timer) {
	gettimeofday(&timer->startTime, NULL);
}

void stopTimer(Timer *timer) {
	gettimeofday(&timer->stopTime, NULL);
}

double timeElapsed(Timer *timer) {
	return ((timer->stopTime.tv_sec-timer->startTime.tv_sec)*1000000LL + timer->stopTime.tv_usec-timer->startTime.tv_usec)/1000.0f; 
}