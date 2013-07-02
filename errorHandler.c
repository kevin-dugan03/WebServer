/*
 * errorHandler.c
 *
 *  Created on: Oct 17, 2012
 *      Author: K Dugan
 */

#include "headerfile.h"
char *getMsg(int);

/*
 * Function: sendError
 * ----------------------------
 *   Is called when an error has occurred and an html error
 *   message needs to be sent to the requestor. Once processed,
 *   sends error message to the requestor through the socket.
 *
 *	 Parameters:
 *   sockfd: The active connection
 *   errorCode: The specific error code for the error that occurred
 *
 *   Returns: nothing
 */
void sendError(int sockfd, int errorCode)
{
	// Variables to hold data to be sent back via error message
	char *httpVersion = "HTTP/1.1";
	char *contentType = "text/html";
	char *error_msg = getMsg(errorCode);
	char response[500];
	char responseText[500];
	char dateAndTime[50];

	// Stores messages to be logged
	char logbuff[BUFSIZE];

	// Get the current date and time
	getTimestamp2(dateAndTime);

	// Create the html to be returned
	int responseTextSize = sprintf(responseText,
			"<html><head><title>%i</title></head><body>%s</body></html>",
			errorCode, error_msg);

	// Create the header reponse for the error message
	int size = sprintf(response,
			"%s %s\nDate: %s\nContent-Type: %s\nContent-Length: %i\r\n\r\n%s\n",
			httpVersion, error_msg, dateAndTime, contentType, responseTextSize + 1, responseText);

	// Log the error, send the error back to the client, close the socket
	sprintf(logbuff, "Error '%s' sent to socket %i.", error_msg, sockfd);
	logger(logbuff);
	send(sockfd, response, size, 0);
	close(sockfd);
}

/*
 * Function: getMsg
 * ----------------------------
 *   Retrieves the appropriate message for the specified error
 *   code.
 *
 *	 Parameters:
 *   code: The error code
 *
 *   Returns: The error message string
 */
char * getMsg(int code)
{
	char * msg;

	switch(code)
	{
		case 400:
			msg = "400 Bad Request";
			break;
		case 403:
			msg = "403 Forbidden - File too large";
			break;
		case 404:
			msg = "404 Not Found";
			break;
		case 405:
			msg = "405 Method Not Allowed";
			break;
		case 415:
			msg = "415 Unsupported Media Type";
			break;
		default:
			msg = "An error has occurred";
			break;
	}

	return msg;
}
