/*
 * handlerRouter.c
 *
 * Contains the function that routes requests to the appropriate
 * GET/POST/HEAD method handler for processing. This function receives
 * the socket file descriptor for the connection, parses the request, confirms
 * it is valid, and then pushes the request to a processing handler.
 *
 * Kevin Dugan
 * 10/10/2012
 */

#include "headerfile.h"

/*
 * Function: router
 * ----------------------------
 *   Receives the HTTP request, conducts basic error checking,
 *   and routes the request to the appropriate handler function
 *   based on the method being used in the request.
 *
 *	 Parameters:
 *   sockfd: The socket identifier of the active connection.
 *
 *   Returns: nothing
 */
void *router(void *socket)
{
	char logbuff[300];
	int sockfd = (int) socket;

	// Variables
	long buffer_bytes;	// Number of bytes in the buffer
	static char buffer[BUFSIZE + 1];	// Buffer to hold request string

	// Receive the request information, place into buffer

	buffer_bytes = recv(sockfd, buffer, BUFSIZE, 0);


	// Check that the request is not empty and that the BUFSIZE has not been exceeded
	if (buffer_bytes <= 0 || buffer_bytes > BUFSIZE)
	{
		// Log error, send error
		if (buffer_bytes <= 0)
		{
			logger("Buffer size is <= 0");
		}

		if (buffer_bytes > BUFSIZE)
		{
			logger("Buffer is larger than allowed buffer size");
		}
		sendError(sockfd, 400);
		pthread_exit(NULL);
		return 0;
	}

	// Check for a valid request method is being used
	if (!strncmp(buffer, "GET ", 4))
	{
		// Log GET request, check formatting of request, call process method
		sprintf(logbuff, "Thread %u: Processing GET request", (unsigned int) pthread_self());
		logger(logbuff);
		processGet(sockfd, buffer);
		bzero(buffer, BUFSIZE);
		return 0;
	}
	else if (!strncmp(buffer, "HEAD ", 5))
	{
		// Log HEAD request, check formatting of request, call process method
		sprintf(logbuff, "Thread %u: Processing HEAD request", (unsigned int) pthread_self());
		logger(logbuff);
		processGet(sockfd, buffer);
		bzero(buffer, BUFSIZE);
		return 0;
	}
	else if (!strncmp(buffer, "POST ", 5))
	{
		// Log POST request, check formatting of request, call process method
		sprintf(logbuff, "Thread %u: Processing POST request", (unsigned int) pthread_self());
		logger(logbuff);
		logger("Processing POST request");
		processPost(sockfd, buffer);
		bzero(buffer, BUFSIZE);
		return 0;
	}
	else
	{
		// Log invalid HTTP request, send error
		logger("Invalid HTTP request method submitted");
		sendError(sockfd, 405);
		pthread_exit(NULL);
		return 0;
	}
}
