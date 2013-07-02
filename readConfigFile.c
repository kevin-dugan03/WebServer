/*
 * readConfigFile.c
 *
 *  Jeff Gore
 *  10/23/2012
 *  Version 1.0
 */

#include "headerfile.h"

// Function prototypes
void getNameValuePair(char *, char *, char *);
void getExtensionTypePair(char *, char *, char *);
int addFiletype(char *);
void initFiletypeArray();
void printFiletypeArrayElement(int);
void printLoadedFiletypes();
void logLoadedFiletypes();

/*
 * Function: getNameValuePair
 * ----------------------------
 *   Function to parse name/value pairs from a line of the config file.
 * 	 Parameters are passed by reference from the caller.
 *
 *	 Parameters:
 *   line - a config file line
 *   name - the name portion of the name/value pair
 *   value - the value portion of the name/value pair
 *
 *   Returns: nothing
 */
void getNameValuePair(char *line, char *name, char *value)
{
	int i, j; 	// index variables

	// Parse the name
	i = 0;
	while (line[i] != NV_DELIMITER)
	{
		name[i] = line[i];
		i++;
	}
	name[i] = '\0';

	// Parse the value
	i++;
	j = 0;
	while (line[i] != '\0' && line[i] != '\n' && line[i] != '\r')
	{
		value[j] = line[i];
		i++;
		j++;
	}
	value[j] = '\0';
}

/*
 * Function: getExtensonTypePair
 * ----------------------------
 *   Function to parse extension/type pairs from the "value" part of a name/value pair.
 * 	 Parameters are passed by reference from the caller.
 *
 *	 Parameters:
 *   pair - the "value" part of a name/value pair from the config file
 *   extension - the extension portion of the extension/type pair
 *   type - the type portion of the extension/type pair
 *
 *   Returns: nothing
 */
void getExtensionTypePair(char *pair, char *extension, char *type)
{
	int i, j; 	// index variables

	// Parse the name
	i = 0;
	while (pair[i] != ET_DELIMITER)
	{
		extension[i] = pair[i];
		i++;
	}
	extension[i] = '\0';

	// Parse the value
	i++;
	j = 0;
	while (pair[i] != '\0' && pair[i] != '\n' && pair[i] != '\r')
	{
		type[j] = pair[i];
		i++;
		j++;
	}
	type[j] = '\0';
}

/*
 * Function: addFileType
 * ----------------------------
 *   Adds a file type extension/type pair to the filetypes array.
 *
 *	 Parameters:
 *   etpair - the "unsplit" extension/type pair
 *
 *   Returns: the index of the filetypes array where the data was added
 */
int addFiletype(char *etpair)
{
	char extensionbuff[BUFSIZE]; // the file extension
	char typebuff[BUFSIZE]; // the file type
	int i;

	extensionbuff[0] = '\0';
	typebuff[0] = '\0';

	// Split etpair into extension and type
	getExtensionTypePair(etpair, extensionbuff, typebuff);

	// Find next available slot in array
	for (i = 0; filetypes[i].index != -1 && i < FILETYPES_ARRAY_SIZE; i++)
	{
		// do nothing
	}

	// Insert into array at the open slot
	if (i < FILETYPES_ARRAY_SIZE)
	{
		filetypes[i].index = i;
		strcpy(filetypes[i].extension, extensionbuff);
		strcpy(filetypes[i].type, typebuff);
	}

	return i;
}

/*
 * Function: initFiletypeArray
 * ----------------------------
 *   Initializes the elements in the the filetypes array.
 *
 *	 Parameters: none
 *
 *   Returns: nothing
 */
void initFiletypeArray()
{
	int i;

	for (i = 0; i < FILETYPES_ARRAY_SIZE; i++)
	{
		filetypes[i].index = -1;
		strcpy(filetypes[i].extension, "0");
		strcpy(filetypes[i].type, "0");
	}
}

/*
 * Function: printFiletypeArrayElement
 * ----------------------------
 *   Prints a filetypes array element to the console.
 *
 *	 Parameters:
 *	 index - the index containing the element to access
 *
 *   Returns: nothing
 */
void printFiletypeArrayElement(int index)
{
	printf("%d>\t %s \t %s\n", filetypes[index].index, filetypes[index].extension, filetypes[index].type);
}

/*
 * Function: printLoadedFiletypes
 * ----------------------------
 *   Prints the loaded file types to the console.
 *
 *	 Parameters: none
 *
 *   Returns: nothing
 */
void printLoadedFiletypes()
{
	int i = 0;

	printf("\n");
	printf("File types successfully loaded.\n");

	for (i = 0; filetypes[i].index != -1 && i < FILETYPES_ARRAY_SIZE; i++)
	{
		printf("%d>\t ext: %s\t type: %s\n", i, filetypes[i].extension, filetypes[i].type);
	}

	printf("\n");
}

/*
 * Function: logLoadedFiletypes
 * ----------------------------
 *   Lists the loaded file types to the log file.
 *
 *	 Parameters: none
 *
 *   Returns: nothing
 */
void logLoadedFiletypes()
{
	int i = 0;
	char logbuff[BUFSIZE];		// the log buffer

	logger("File types successfully loaded.");

	for (i = 0; filetypes[i].index != -1 && i < FILETYPES_ARRAY_SIZE; i++)
	{
		sprintf(logbuff, "%d>\t ext: %s\t type: %s", i, filetypes[i].extension, filetypes[i].type);
		logger(logbuff);
	}
}

/*
 * Function to read and process the configuration file.
 */
/*
 * Function: readConfigFile
 * ----------------------------
 *   Reads and processes the configuration file.
 *
 *	 Parameters:
 *	 filename - the configuration file name
 *	 port - the port
 *	 dir - the home directory
 *
 *   Returns: 0 for no error, >0 for error
 */
int readConfigFile(char *filename, char *port, char *dir)
{
	char fileline[BUFSIZE];		// line from the file
	char namebuff[BUFSIZE];		// name buffer
	char valuebuff[BUFSIZE];	// value buffer
	char logbuff[BUFSIZE];		// the log buffer

	printf("Config file name: %s\n", filename);

	// Initialize the filetypes array
	initFiletypeArray();

	// Open the file for reading
	FILE *configFile = fopen(filename, "r");

	// File not found or problem with file open
	if (configFile == NULL)
	{
		logger("Configuration file not found.");
		printf("Configuration file not found.\n");
		return(CONFIG_FILE_ERR);
	}
	else // File open successful
	{
		sprintf(logbuff, "Configuration file found: %s", filename);
		logger(logbuff);
		logger("Processing configuration file...");
		printf ("Configuration file found: %s\n", filename);
		printf ("Processing configuration file...\n");

		// Main loop to read the config file line-by-line.
		// Get a line from the file
		while (fgets (fileline, BUFSIZE, configFile) != NULL)
		{
			// Skip blank lines or comment lines.
			if (fileline[0] != '\r' && fileline[0] != '\n' && fileline[0] != '/' && fileline[0] != '#')
			{
				// Get the name/value pair in the line
				getNameValuePair(fileline, namebuff, valuebuff);

				// If this is a port line
				if (!strcmp(namebuff, "port"))
				{
					strcpy(port, valuebuff);
				}

				// If this is a directory line
				if (!strcmp(namebuff, "home"))
				{
					strcpy(dir, valuebuff);
				}

				// If this is a mimetype line
				if (!strcmp(namebuff, "mimetype"))
				{
					// printf("Called addFiletype\n");
					int i = -1;
					i = addFiletype(valuebuff);
					//printFiletypeArrayElement(i);
				}
			}
		}

		printLoadedFiletypes();
		logLoadedFiletypes();

		// Close the file
		fclose (configFile);

		return(0);
	}
}
