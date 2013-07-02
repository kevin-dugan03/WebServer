/*
 * log.c
 *
 *  Created on: Oct 26, 2012
 *      Author: K Dugan
 */

#include "headerfile.h"

/*
 * Function: logger
 * ----------------------------
 *   Logs transactions from the Mini Web Server as it
 *   processes requests
 *
 *	 Parameters:
 *   message: The message to add to the log file.
 *
 *   Returns: nothing
 */
void logger(char *message)
{
	int logfile_fd;		// The log file identifier
	char logbuffer[BUFSIZE];	// The message buffer
	char dateAndTime[29];	//the date and time string

	// Get the current date and time
	getTimestamp2(dateAndTime);

	// Open the log file, create if needed, add messages to the log
	if((logfile_fd = open(logfilePathAndName, O_CREAT | O_WRONLY | O_APPEND, 0644)) >= 0)
	{
		sprintf(logbuffer, "%s: %s\n", dateAndTime, message);
		write(logfile_fd, logbuffer, strlen(logbuffer));
		close(logfile_fd);
	}
}
