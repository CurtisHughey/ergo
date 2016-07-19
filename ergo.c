#include "ergo.h"

int main(int argc, char **argv) {
	if (argc <= 1) {
		ERROR_PRINT("Must provide command line arguments (-h for help)");
		exit(1);
	}

	int customConfig = 0;
	int rollouts = -1;
	char *configFileName;
	functions function = NONE;

	int opt = 0;
	while ((opt = getopt(argc, argv, "C:r:upcxytgh")) != -1) {  // Add more options later
		switch (opt) {
			case 'C':
				customConfig = 1;
				configFileName = optarg;
				break;
			case 'r':
				rollouts = atoi(optarg);
				break;
			case 'u':
				function = UNIT;
				break;
			case 'p':
				function = HVH;
				break;
			case 'c':
				function = HVC;
				break;
			case 'x':
				function = CVC;
				break;
			case 'y':
				function = CVR;
				break;
			case 't':
				function = TIME;
				break;
			case 'g':
				function = GTP;
				break;
			case 'h':
				printf("Options\n");
				printf("\t-C\n");
				printf("\t\tUse a custom configuration file\n\n");
				printf("\t-r\n");
				printf("\t\tSpecify the number of rollouts\n\n");
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
				printf("\t-t\n");
				printf("\t\tRun GTP protocol (used for playing on KGS)\n\n");
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

	if (rollouts > 0) {
		config->rollouts = rollouts;
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
			runHumanVsComputer(config->rollouts, config->averageLengthOfGame);
			break;
		case CVC:
			runComputerVsComputer(config->rollouts, config->averageLengthOfGame);
			break;
		case CVR:
			return testComputer(config->tests, config->rollouts, config->averageLengthOfGame);
			break;
		case TIME:
			timeTrials(config->warmupTrials, config->trials, config->rollouts, config->averageLengthOfGame);
			break;
		case GTP:
			return runGtp(config->rollouts, config->averageLengthOfGame)+2;  // +2 to make it non-negative.  0 for failure, 1 for loss, 2 for draw, 3 for win, should log
			break;
		case NONE:
		default:
			ERROR_PRINT("Function not specified\n");
			break;
	}

	destroyConfig(config);

	return result;
}