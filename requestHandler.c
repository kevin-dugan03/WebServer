#include "headerfile.h"

/*
 * Function: getContentType
 * ----------------------------
 *   Translates the requested file's extension into a MIME type
 *
 *	 Parameters:
 *   resource: The file being requested with filetype extension
 *
 *   Returns: MIME type of the file being requested
 */
char* getContentType(char *resource)
{
	int periodLoc = strlen(resource);
	for (; resource[periodLoc] != '.'; periodLoc--)
		;

	char* contentType = "\0";

	int i;
	for (i = 0; filetypes[i].index != -1 && i < FILETYPES_ARRAY_SIZE; i++)
	{
		if (strcmp(filetypes[i].extension, resource + periodLoc + 1) == 0)
		{
			contentType = strdup(filetypes[i].type);
			break;
		}
	}
	return contentType;
}

/*
 * Function: getResourceName
 * ----------------------------
 *   Gets the name of the requested resource
 *
 *	 Parameters:
 *   resourceName: The string to store the resource name into
 *   requestData: The data from the request
 */
void getResourceName(char *resourceName, char *requestData)
{
	char *resourceStart = strchr(requestData, ' ') + 1;

	// if we're not starting with a "/" we have an absolute URL
	if (*resourceStart != '/') {
		// move past http://
		resourceStart += 7;
		// look for the next "/"
		resourceStart = strchr(resourceStart, '/');
	}

	resourceStart++;

	// find potential form data
	char *resourceEnd = strchr(resourceStart, '?');

	// if we didn't find form data or there is a space before the form data
	// use that location instead
	if (resourceEnd == NULL || strchr(resourceStart, ' ') < resourceEnd)
	{
		resourceEnd = strchr(resourceStart, ' ');
	}

	// if we have a resource, use it
	if (resourceEnd - resourceStart > 0)
	{
		strncpy(resourceName, resourceStart, resourceEnd - resourceStart);
	}
	else
	{
		// otherwise use the default
		strcpy(resourceName, DEFAULT_START);
	}

	// log
	char logbuff[BUFSIZE];
	sprintf(logbuff, "Thread %u: Resource requested: %s.", (unsigned int) pthread_self(), resourceName);
	logger(logbuff);
}

/*
 * Function: getFormData
 * ----------------------------
 *   Gets form data, if any
 *
 *	 Parameters:
 *   formData[]: A 3 slot char * array for the form data to be placed into.
 *   requestData: The data from the request
 */
void getFormData(char *formData[], char *requestData)
{
	char *formDataStart;
	char *formDataEnd;

	char *p;

	if (!strncmp(requestData, "GET ", 4))
	{
		formDataStart = strchr(requestData, '?');
		formDataEnd = strchr(requestData + 4, ' ');

		if (formDataStart != NULL && formDataStart < formDataEnd)
		{
			formData[0] = strchr(formDataStart, '=') + 1;
			formData[1] = strchr(formData[0], '=') + 1;
			formData[2] = strchr(formData[1], '=') + 1;

			p = strchr(formData[0], '&');
			p[0] = '\0';
			p = strchr(formData[1], '&');
			p[0] = '\0';
			p = strchr(formData[2], ' ');
			p[0] = '\0';
		}
	}
	else if (!strncmp(requestData, "POST ", 5))
	{
		formDataStart = strrchr(requestData, '\n');

		formData[0] = strchr(formDataStart, '=') + 1;
		formData[1] = strchr(formData[0], '=') + 1;
		formData[2] = strchr(formData[1], '=') + 1;

		p = strchr(formData[0], '&');
		p[0] = '\0';
		p = strchr(formData[1], '&');
		p[0] = '\0';
		// no need to terminate last one since it already
		// is at the end of the requestData
	}

	if (formData[0] != NULL)
	{
		// reformat email
		if (strstr(formData[2], "%40") != NULL)
		{
			char *part1 = strtok(formData[2], "%40");
			char *part2 = strtok(NULL, "%40");
			sprintf(formData[2], "%s@%s", part1, part2);
		}
		// log
		char logbuff[BUFSIZE];
		sprintf(logbuff, "Thread %u: Form data found: %s, %s, %s.",
				(unsigned int) pthread_self(), formData[0], formData[1], formData[2]);
		logger(logbuff);
	}
	else
	{
		char logbuff[BUFSIZE];
		sprintf(logbuff, "Thread %u: No form data found.", (unsigned int) pthread_self());
		logger(logbuff);
	}
}

/*
 * Function: getResponseSize
 * ----------------------------
 *   Gets the size of the response
 *
 *	 Parameters:
 *   resourceName: The path of the resource.
 *   formData[]: Form data, if any
 *   socket: The socket to write to if the file cannot be sent.
 *
 *   Returns: the size of the response or -1 if an error was thrown and the socket closed.
 */
int getResponseSize(char *resourceName, char *formData[], int socket)
{
	int result = -1;
	char logbuff[BUFSIZE];

	FILE *targetFile = fopen(resourceName, "r");
	if (targetFile != NULL)
	{
		fseek(targetFile, 0, SEEK_END);
		result = ftell(targetFile);
		fclose(targetFile);

		sprintf(logbuff, "Thread %u: - %s - found with size: %i", (unsigned int) pthread_self(), resourceName, result);
		logger(logbuff);

		if (formData[0] != NULL)
		{
			result += strlen(formData[0]) + strlen(formData[1]) + strlen(formData[2]);
			result -= 6;
		}

		// Check to ensure it's smaller than the max file size, log message
		if (result <= MAX_GET_REQUEST_SIZE)
		{
			sprintf(logbuff, "Thread %u: - %s - can be sent.", (unsigned int) pthread_self(), resourceName);
			logger(logbuff);
		}
		else
		{
			sprintf(logbuff, "Thread %u: - %s - Too large, can NOT be sent.", (unsigned int) pthread_self(), resourceName);
			logger(logbuff);
			sendError(socket, 403);
		}
	}
	else
	{
		sprintf(logbuff, "Thread %u: - %s - not found.", (unsigned int) pthread_self(), resourceName);
		logger(logbuff);
		sendError(socket, 404);
	}
	return result;
}

/*
 * Function: sendResponseHeader
 * ----------------------------
 *   Sends the header part of the response.
 *
 *	 Parameters:
 *   resourceName: The path of the resource.
 *   contentType: The mime type for the content.
 *   responseSize: The size of the response.
 *   socket: The socket to send the response to.
 */
void sendResponseHeader(char *resourceName, char *contentType, int responseSize, int socket)
{
	char response[200];
	char logbuff[BUFSIZE];

	char dateAndTime[29];
	getTimestamp2(dateAndTime);

	// Craft response for a file
	int size = sprintf(response,
			"HTTP/1.1 200 OK\nDate: %s\nContent-Type: %s\nContent-Length: %i\r\n\r\n",
			dateAndTime, contentType, responseSize);

	sprintf(logbuff, "Thread %u: Sent header information to socket %i", (unsigned int) pthread_self(), socket);
	logger(logbuff);
	write(socket, response, size);
}

/*
 * Function: sendData
 * ----------------------------
 *   Sends data to the socket.
 *
 *	 Parameters:
 *   resourceName: The resource to be sent.
 *   formData[]: the data for the form
 *   socket: The socket to send the data to.
 */
void sendData(char *resourceName, char *formData[], int socket)
{
	char logbuff[BUFSIZE];
	char buffer[BUFSIZE];
	int bufferCount;
	bzero(buffer, BUFSIZE);

	FILE *readableFile = fopen(resourceName, "r");

	sprintf(logbuff, "Thread %u: Sending file information to socket %i", (unsigned int) pthread_self(), socket);
	logger(logbuff);

	// Write out the file to the socket
	while ((bufferCount = read(fileno(readableFile), buffer, BUFSIZE)) > 0)
	{
		if (formData[0] == NULL)
		{
			//sprintf(logbuff, "Sending block of data.");
			//logger(logbuff);
			write(socket, buffer, bufferCount);
		}
		else
		{
			//sprintf(logbuff, "Sending block of form data.");
			//logger(logbuff);
			char bufferWithData[BUFSIZE];
			sprintf(bufferWithData, buffer, formData[0], formData[1], formData[2]);
			write(socket, bufferWithData, strlen(bufferWithData));
		}

		bzero(buffer, BUFSIZE);
	}

	// Close the file
	fclose(readableFile);
}

/*
 * Function: processGet
 * ----------------------------
 *   Call to process GET requests
 *
 *	 Parameters:
 *   socket: The socket to send data out to.
 *   requestData: The data from the request.
 */
void processGet(int socket, char *requestData)
{
	char resourceName[strlen(requestData)];
	bzero(resourceName, strlen(requestData));
	getResourceName(resourceName, requestData);

	char *formData[3];
	formData[0] = NULL;
	getFormData(formData, requestData);

	int responseSize = getResponseSize(resourceName, formData, socket);

	if (responseSize != -1)
	{
		char *contentType = getContentType(resourceName);

		if (strlen(contentType) == 0)
		{
			sendError(socket, 415);
			return;
		}

		sendResponseHeader(resourceName, contentType, responseSize, socket);
		sendData(resourceName, formData, socket);
		close(socket);
	}
}

/*
 * Function: processHead
 * ----------------------------
 *   Call to process HEAD requests
 *
 *	 Parameters:
 *   socket: The socket to send data out to.
 *   requestData: The data from the request.
 */
void processHead(int socket, char *requestData)
{
	char resourceName[strlen(requestData)];
	getResourceName(resourceName, requestData);

	int responseSize = getResponseSize(resourceName, NULL, socket);

	if (responseSize != -1)
	{
		char *contentType = getContentType(resourceName);

		if (strlen(contentType) == 0)
		{
			sendError(socket, 415);
			return;
		}

		sendResponseHeader(resourceName, contentType, responseSize, socket);
		close(socket);
	}
}

/*
 * Function: processPost
 * ----------------------------
 *   Call to process POST requests
 *
 *	 Parameters:
 *   socket: The socket to send data out to.
 *   requestData: The data from the request.
 */
void processPost(int socket, char *requestData)
{
	char resourceName[strlen(requestData)];
	bzero(resourceName, strlen(requestData));
	getResourceName(resourceName, requestData);

	char *formData[3];
	formData[0] = NULL;
	getFormData(formData, requestData);

	int responseSize = getResponseSize(resourceName, formData, socket);

	if (responseSize != -1)
	{
		char *contentType = getContentType(resourceName);

		if (strlen(contentType) == 0)
		{
			sendError(socket, 415);
			return;
		}

		sendResponseHeader(resourceName, contentType, responseSize, socket);
		sendData(resourceName, formData, socket);
		close(socket);
	}
}
