/*
 * main.c
 *
 * Contains the main function for the application.
 *
 * Jeff Gore
 * 10/20/2012
 * version 1.0
 */

#include "headerfile.h"

// Function prototypes
int isValidPort(char *);
int isEmptyString(char *);
int setPort(char *, char *);
int setHomeDir(char *, char *);

// Initialize global variables
filetypes_template filetypes[FILETYPES_ARRAY_SIZE];
char logfilePathAndName[BUFSIZE];

/*
 * Function: isValidPort
 * ----------------------------
 *   Determines if a port number is valid or not.
 *
 *	 Parameters:
 *   port - the port to test
 *
 *   Returns: 0 if not a valid port, an integer equal to the port number otherwise
 */
int isValidPort(char * port) {
	int portInteger;

	// Convert cstring to integer
	portInteger = atoi(port);

	if (portInteger == 0 || portInteger < PORT_MIN || portInteger > PORT_MAX) {
		return (0); // false
	}

	return (portInteger); // true
}

/*
 * Function: isEmptyString
 * ----------------------------
 *   Determines if a cstring is empty.
 *
 *	 Parameters:
 *   cstring - the cstring to test
 *
 *   Returns: 0 if not empty, 1 if empty
 */
int isEmptyString(char * cstring) {
	if (cstring[0] == '\0') {
		return (1);
	} else {
		return (0);
	}
}

/*
 * Function: setPort
 * ----------------------------
 *   Sets the port number.
 *
 *	 Parameters:
 *   commandLinePort - the command line port
 *   configFilePort - the config file port
 *
 *   Returns: the port number if set or -1 if not
 */
int setPort(char * commandLinePort, char * configFilePort) {
	int intPort = -1;
	char logbuff[BUFSIZE]; // the log buffer

	// Check if command line port is valid.
	intPort = isValidPort(commandLinePort);

	// If not valid
	if (intPort == 0) {
		if (isEmptyString(commandLinePort)) {
			logger("No port found on command line.");
			printf("No port found on command line.\n");
		} else {
			sprintf(logbuff, "Invalid command line port: %s", commandLinePort);
			logger(logbuff);
			printf("Invalid command line port: %s\n", commandLinePort);
		}

		// Check if config file port is valid.
		intPort = isValidPort(configFilePort);

		// If not valid
		if (intPort == 0) // not valid
				{
			if (isEmptyString(configFilePort)) {
				logger("No port found in configuration file.");
				printf("No port found in configuration file.\n");
			} else {
				sprintf(logbuff, "Invalid configuration file port: %s",
						configFilePort);
				logger(logbuff);
				printf("Invalid configuration file port: %s\n", configFilePort);
			}

			// Use default port
			intPort = DEFAULT_PORT;
			sprintf(logbuff, "Using default port: %d", intPort);
			logger(logbuff);
			printf("Using default port: %d\n", intPort);
		} else {
			sprintf(logbuff, "Using configuration file port: %d", intPort);
			logger(logbuff);
			printf("Using configuration file port: %d\n", intPort);
		}
	} else {
		sprintf(logbuff, "Using command line port: %d", intPort);
		logger(logbuff);
		printf("Using command line port: %d\n", intPort);
	}

	return intPort;
}

/*
 * Function: setHomeDir
 * ----------------------------
 *   Sets the home directory.
 *
 *	 Parameters:
 *   commandLineDir - the command line home directory
 *   configFileDir - the config file home directory
 *
 *   Returns: the port number if set or -1 if not
 */
int setHomeDir(char * commandLineDir, char * configFileDir) {
	char logbuff[BUFSIZE]; // the log buffer

	// Check if command line home dir is valid
	if (chdir(commandLineDir) == -1) // not valid
			{
		if (isEmptyString(commandLineDir)) {
			// No command line home dir
			logger("No home directory found on command line.");
			printf("No home directory found on command line.\n");
		} else {
			// Can't change to command line home dir
			sprintf(logbuff, "Can't change to command line directory %s",
					commandLineDir);
			logger(logbuff);
			printf("Can't change to command line directory %s\n",
					commandLineDir);
		}

		// Check if config file dir is valid
		if (chdir(configFileDir) == -1) // not valid
				{
			// No config file home dir
			if (isEmptyString(configFileDir)) {
				logger("No home directory found in configuration file.");
				printf("No home directory found in configuration file.\n");
			} else {
				// Can't change to config file home dir
				sprintf(logbuff,
						"Can't change to configuration file directory %s",
						configFileDir);
				logger(logbuff);
				printf("Can't change to configuration file directory %s\n",
						configFileDir);
			}

			if (chdir(DEFAULT_DIR) == -1) // not valid
					{
				// Can't change to default home dir
				sprintf(logbuff, "Can't change to default directory %s",
						DEFAULT_DIR);
				logger(logbuff);
				printf("Can't change to default directory %s\n", DEFAULT_DIR);
				return (DIR_ERR);
			} else // valid
			{
				// Using default home dir
				sprintf(logbuff, "Using default directory: %s", DEFAULT_DIR);
				logger(logbuff);
				printf("Using default directory: %s\n", DEFAULT_DIR);
			}
		} else // valid
		{
			// Using config file home dir
			sprintf(logbuff, "Using configuration file directory: %s",
					configFileDir);
			logger(logbuff);
			printf("Using configuration file directory: %s\n", configFileDir);
		}
	} else // valid
	{
		// Using command line home dir
		sprintf(logbuff, "Using command line directory: %s", commandLineDir);
		logger(logbuff);
		printf("Using command line directory: %s\n", commandLineDir);
	}

	// Print current working directory
	sprintf(logbuff, "Current working directory is %s", get_current_dir_name());
	logger(logbuff);
	printf("Current working directory is %s\n", get_current_dir_name());

	return (0);
}

/*
 * Function: createDefaultConfigFile
 * -----------------------------------------
 * 	  Creates a default config file in the directory
 * 	  that the web server is launched from if it doesn't
 * 	  already exist.
 */
void createDefaultConfigFile() {
	FILE *configFile = fopen(DEFAULT_CONFIG_FILE, "r");
	if (configFile == NULL ) {
		printf("Attempting to create a config file.\n");

		configFile = fopen(DEFAULT_CONFIG_FILE, "w");

		if (configFile == NULL ) {
			printf("Could not create config file.\n");
			return;
		}

		fputs(
				"// If first character of line is '/' or '#' the line is considered a comment.\n",
				configFile);
		fputs(
				"// The '=' is the delimiter for name/value pairs as in \"name=value\".\n",
				configFile);
		fputs(
				"// The '&' is the delimiter for extension/type pairs as in \"extension&type\".  This\n",
				configFile);
		fputs(
				"// applies only to lines where the name/value pair name is \"mimetype\".\n",
				configFile);
		fputs(
				"// There should be no spaces at the beginning of a line (except for blank lines) and\n",
				configFile);
		fputs("// no spaces on either side of a delimiter.\n\n", configFile);
		fputs("port=5555\n", configFile);
		fputs("home=", configFile);
		fputs(get_current_dir_name(), configFile);
		fputs("\n\n", configFile);
		fputs("mimetype=css&text/css\n", configFile);
		fputs("mimetype=doc&application/doc\n", configFile);
		fputs("mimetype=docx&application/docx\n", configFile);
		fputs("mimetype=gif&image/gif\n", configFile);
		fputs("mimetype=gz&image/gz\n", configFile);
		fputs("mimetype=html&text/html\n", configFile);
		fputs("mimetype=htm&text/html\n", configFile);
		fputs("mimetype=ico&image/ico\n", configFile);
		fputs("mimetype=jpeg&image/jpeg\n", configFile);
		fputs("mimetype=jpg&image/jpg\n", configFile);
		fputs("mimetype=js&application/javascript\n", configFile);
		fputs("mimetype=pdf&application/pdf\n", configFile);
		fputs("mimetype=png&image/png\n", configFile);
		fputs("mimetype=pptx&application/pptx\n", configFile);
		fputs("mimetype=tar&image/tar\n", configFile);
		fputs("mimetype=txt&text/plain\n", configFile);
		fputs("mimetype=zip&image/zip\n", configFile);
	}

	fclose(configFile);
}

/*
 * Function: main
 * ----------------------------
 *   Initiates the Mini Web Server, processes the command line arguments
 *   and the configuration file, and calls the listener function to
 *   begin listening for connections.
 *
 *	 Parameters:
 *   program name - the executable program name
 *   config file - the configuration file name
 *   port number - the port number to use
 *   root directory - the root directory for the server to use
 *
 *   Returns: listener code
 */
int main(int argc, char **argv) {
	int i,                  // loop var
			listenerReturnCode, // return code from listener
			configFileResult,	// return code from reading config file
			port,				// the port number
			setDirReturnCode;	// return code from setHomeDir// the port number
	char configFilePort[BUFSIZE] = "\0";  // config file port number buffer
	char configFileDir[BUFSIZE] = "\0";   // config file home directory buffer
	char commandLinePort[BUFSIZE] = "\0"; // command line port buffer
	char commandLineDir[BUFSIZE] = "\0";  // command line home dir buffer
	char logbuff[BUFSIZE];				  // the log buffer

	// Get the startup directory
	sprintf(logfilePathAndName, "%s", get_current_dir_name());
	// Concatenate the log file name
	strcat(logfilePathAndName, LOGFILE);
	// Print log file path and name to console
	printf("Log file path and name: %s\n", logfilePathAndName);

	// Log program initiation
	logger("Mini Web Server started");

	// Log command line arguments.
	for (i = 0; i < argc; i++) {
		sprintf(logbuff, "Arg %d: %s", i, argv[i]);
		logger(logbuff);
	}

	// If too many command line arguments.
	if (argc > CMD_LINE_ARG_MAX) {
		// Print console messages and exit.
		logger("Incorrect number of command line arguments.Program ending.");
		printf(
				"Incorrect number of command line arguments.\nProgram ending.\n");
		exit(CMD_LINE_ERR);
	}

	// Log PID to console.
	sprintf(logbuff, "PID = %d", getpid());
	logger(logbuff);

	// Read the configuration file.
	if (argc > 1) {
		configFileResult = readConfigFile(argv[1], configFilePort,
				configFileDir);
	} else {
		createDefaultConfigFile();

		configFileResult = readConfigFile(DEFAULT_CONFIG_FILE, configFilePort,
				configFileDir);
	}

	// If config file can't be found or opened
	if (configFileResult != 0) {
		// If error, print console message and exit.
		logger("Abnormal program end.");
		printf("Abnormal program end.\n");
		exit(configFileResult);
	}

	// Set port and home directory
	switch (argc) {
	case 1: // <program name>
		port = setPort(commandLinePort, configFilePort);
		setDirReturnCode = setHomeDir(commandLineDir, configFileDir);
		break;
	case 2: // <program name> <config file name>
		port = setPort(commandLinePort, configFilePort);
		setDirReturnCode = setHomeDir(commandLineDir, configFileDir);
		break;
	case 3: // <program name> <config file name> <port>
		port = setPort(argv[2], configFilePort);
		setDirReturnCode = setHomeDir(commandLineDir, configFileDir);
		break;
	case 4: // <program name> <config file name> <port> <home directory>
		port = setPort(argv[2], configFilePort);
		setDirReturnCode = setHomeDir(argv[3], configFileDir);
		break;
	}

	sprintf(logbuff, "Port is set to: %d", port);
	logger(logbuff);
	printf("Port is set to: %d\n", port);

	// If problem with setting home directory
	if (setDirReturnCode != 0) {
		logger("Abnormal program end.");
		printf("Abnormal program end.\n");
		exit(setDirReturnCode);
	}

	// For testing
	//exit(0);

	// Call listener function.
	listenerReturnCode = listener(port);

	// Log end of program status
	if (listenerReturnCode == 0) {
		logger("Normal program end.");
	} else {
		logger("Abnormal program end.");
	}

	return (listenerReturnCode);
}
