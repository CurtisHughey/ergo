#include "ergo.h"

int main(int argc, char **argv) {
	printf("Hello World!\n");

	if (argc <= 1) {
		ERROR_PRINT("Must provide command line arguments (-H for help)");
		exit(1);
	}

	int opt = 0;
	while ((opt = getopt(argc, argv, "upc")) != -1) {  // Add more options later
		switch (opt) {
			case 'u':
				runAllUnitTests();
				break;
			case 'p':
				runHumanVsHuman();
				break;
			case 'c':
				runHumanVsComputer();
				break;
			case 'H':
				printf("Options\n");
				printf("\t-u\n");
				printf("\t\tRun unit tests\n\n");
				printf("\t-p\n");
				printf("\t\tRun player vs player\n\n");			
			case '?':
			default:
				ERROR_PRINT("Unknown option, use -H for help");
				exit(1);
		}
	}
}