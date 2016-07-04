#include "ergo.h"

int main(int argc, char **argv) {
	printf("Hello World!\n");

	if (argc <= 1) {
		ERROR_PRINT("Must provide command line arguments (-h for help)");
		exit(1);
	}

	int opt = 0;
	while ((opt = getopt(argc, argv, "upchxyt")) != -1) {  // Add more options later
		switch (opt) {
			case 'u':
				runAllUnitTests();
				break;
			case 'p':
				runHumanVsHuman();
				break;
			case 'c':
				runHumanVsComputer(1000);
				break;
			case 'x':
				runComputerVsComputer(1000);
				break;
			case 'y':
				testComputer(10, 500);
				break;
			case 't':
				timeTrials(5, 1);
				break;
			case 'h':
				printf("Options\n");
				printf("\t-u\n");
				printf("\t\tRun unit tests\n\n");
				printf("\t-p\n");
				printf("\t\tRun player vs player\n\n");		
				printf("\t-c\n");
				printf("\t\tRun player vs computer\n\n");
				printf("\t-x\n");
				printf("\t\tRun computer vs computer\n\n");
				printf("\t-y\n");
				printf("\t\tRun computer vs other tests\n\n");
				printf("\t-t\n");
				printf("\t\tRun time trials\n\n");
				printf("\t-h\n");
				printf("\t\tThis\n\n");	
				break;	
			case '?':
			default:
				ERROR_PRINT("Unknown option, use -h for help");
				exit(1);
		}
	}
}