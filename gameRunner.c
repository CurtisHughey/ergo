#include "gameRunner.h"

void runHumanVsHuman(void) {
	State *state = createState();

	char *colors[2] = { "Black", "White" };
	UNUSED(colors);

	while (1) {
		for (int i = 0; i < 2; i++) {
			printf(" to move: ");
		}
	}

	destroyState(state);
}