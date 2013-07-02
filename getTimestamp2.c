/*
 * getTimestamp2.c
 *
 * Returns the current system time in RFC1123 format:
 * Sun, 14 Oct 2012 15:07:10 GMT
 *
 * The return is done via call by reference rather than function return value.
 *
 * The calling function does the call something like this:
 * 		char timestampbuff[100];
 * 		getTimestamp2(timestampbuff);
 *		printf("Timestamp: %s\n", timestampbuff);
 *
 * Jeff Gore
 * 10/14/2012
 * Version 1.0
 */


#include <time.h>
#include "headerfile.h"

#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"

/*
 * Returns the current system time in RFC1123 format:
 * Sun, 14 Oct 2012 15:07:10 GMT
 * via call by reference.
 */
void getTimestamp2(char *timestamp)
{
	// Declare variables.
	time_t now;
	char timebuff[100];

	// Get current system time.
	now = time((time_t*) 0);

	// Copies RFC1123FMT into timebuff and expands format tags with GMT values.
	strftime(timebuff, sizeof(timebuff), RFC1123FMT, gmtime(&now));

	// Copy the contents of timebuff to memory loc of timestamp.
	strncpy(timestamp, timebuff, sizeof(timebuff));

}
