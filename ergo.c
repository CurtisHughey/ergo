#include "ergo.h"

int main(int argc, char **argv) {
	printf("Hello World!\n");

	if (argc <= 1) {
		ERROR_PRINT("Must provide command line arguments (-h for help)");
		exit(1);
	}

	int customConfig = 0;
	char *configFileName;
	functions function = NONE;

	int opt = 0;
	while ((opt = getopt(argc, argv, "C:upcxyth")) != -1) {  // Add more options later
		switch (opt) {
			case 'C':
				customConfig = 1;
				configFileName = optarg;
				break;
			case 'u':
				function = UNIT;
				//runAllUnitTests();
				break;
			case 'p':
				function = HVH;
				//runHumanVsHuman();
				break;
			case 'c':
				function = HVC;
				//runHumanVsComputer(config->rollouts);
				break;
			case 'x':
				function = CVC;
				//runComputerVsComputer(config->rollouts);
				break;
			case 'y':
				function = CVR;
				//testComputer(config->tests, config->rollouts);
				break;
			case 't':
				function = TIME;
				//timeTrials(config->trials, config->rollouts);
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

	Config *config = NULL;
	if (customConfig) {
		config = parseConfigFile(configFileName);
	}
	else {
		config = getDefaultConfig();
	}

	int result = 0;
	switch (function) {
		case UNIT:
			result = runAllUnitTests(config->unitRandomMakeUnmakeTests);
			break;
		case HVH:
			runHumanVsHuman();
			break;
		case HVC:
			runHumanVsComputer(config->rollouts);
			break;
		case CVC:
			runComputerVsComputer(config->rollouts);
			break;
		case CVR:
			return testComputer(config->tests, config->rollouts);
			break;
		case TIME:
			return timeTrials(config->trials, config->rollouts);
			break;
		case NONE:
		default:
			ERROR_PRINT("Function not specified\n");
			break;
	}

	destroyConfig(config);

	return result;
}