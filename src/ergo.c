#include "ergo.h"

int main(int argc, char **argv) {
	if (argc <= 1) {
		ERROR_PRINT("Must provide command line arguments (-h for help)");
		exit(1);
	}

	int rollouts = -1;
	int setKomi = 0;  // Command line args sets this to 1 if overriding
	int komiTimes10 = 0;
	char *configFileName = NULL;
	char *zobristDataFileName = NULL;
	functions function = NONE;

	int opt = 0;
	while ((opt = getopt(argc, argv, "C:r:k:z:upcxytgh")) != -1) {  // Add more options later
		switch (opt) {
			case 'C':
				configFileName = optarg;
				break;
			case 'r':
				rollouts = atoi(optarg);
				if (rollouts < 1) {
					ERROR_PRINT("Number of rollouts must be greater than 0, got: %d", rollouts);
					return 1;
				}
				break;
			case 'k':
				setKomi = 1;
				komiTimes10 = atoi(optarg);
				if (komiTimes10 % 10 != 0 && komiTimes10 % 5 != 0) {
					ERROR_PRINT("You specified a 10*komi of %d, must end in 5 or 0.  Exiting.", komiTimes10);
					return 1;
				}
				break;
			case 'z':
				zobristDataFileName = optarg;  // Trusting the argument...
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
				printf("\t-k\n");
				printf("\t\tSpecify the komi, times 10 (e.g. input 75 for a komi of 7.5)\n\n");
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
				ERROR_PRINT("Unknown option %c, use -h for help", opt);
				exit(1);
		}
	}

	Config *config = NULL;
	if (configFileName) {
		config = parseConfigFile(configFileName);
	}
	else {
		config = getDefaultConfig();
	}

	// Config options that may have been overridden on the command line
	if (rollouts > 0) { 
		config->rollouts = rollouts;
	}

	if (!config->superko) {  // Hmmm, I'm not sure where this should get computed ^^^
		config->hashBuckets = 0;  // Overwrites this to prevent hash table
	}

	if (setKomi) {
		config->komiTimes10 = komiTimes10;
	}


	// Set up Zobrist hashing:
	initHashVals(zobristDataFileName);	
	
	int result = 0;
	switch (function) {
		case UNIT:
			result = runAllUnitTests(config);
			break;
		case HVH:
			runHumanVsHuman(config);
			break;
		case HVC:
			runHumanVsComputer(config);
			break;
		case CVC:
			runComputerVsComputer(config);
			break;
		case CVR:
			result = testComputer(config);
			break;
		case TIME:
			timeTrials(config);  // Results written to file
			break;
		case GTP:
			result = runGtp(config)+2;  // +2 to make it non-negative.  0 for failure, 1 for loss, 2 for draw, 3 for win, should log
			break;
		case NONE:
			ERROR_PRINT("Function not specified\n");
			break;
		default:
			ERROR_PRINT("Function %d not recognized\n", function);
			break;
	}

	destroyConfig(config);
	config = NULL;

	return result;
}