/*
 * listener.c
 *
 * This function creates a listener socket, accepts connections, and calls a
 * handler function to handle each connection.
 *
 * Jeff Gore
 * 10/20/2012
 * version 2.0
 */

#include "headerfile.h"
/*
 * Function: listener
 * ----------------------------
 *   Listens for connections on the port parameter.
 *
 *	 Parameters:
 *   port: The port number on which to listen for connections.
 *
 *   Returns: 0 for no error, > 0 for error. Socket errors prior to
 *   the infinite loop are considered "fatal" errors and result in an
 *   immediate return to the caller.
 */
int listener(int port)
{
	int listenersocket,     // The listening socket
	    handlersocket,      // The handler socket
	    count;              // Loop var
	    socklen_t length;   // Length of client addr

	    static struct sockaddr_in client_addr; // Client address structure
	    static struct sockaddr_in server_addr; // Server address structure

	threadpool *pool;
	char logbuff[BUFSIZE];

    // Log server startup message.
    sprintf(logbuff, "Server attempting to start on port %d.", port);
    logger(logbuff);

    // Create the listener socket.
    if((listenersocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // Log error message and exit.
        logger("Error on socket call. Program ending.");
        return(SOCKET_ERR);
    }
    else
    {
    	// Log message for successful socket creation.
        sprintf(logbuff, "Socket id %d successfully created.", listenersocket);
        logger(logbuff);
    }

    // Populate server addr structure.
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // Bind the listener socket.
    if(bind(listenersocket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        // Log error message and exit.
        logger("Error on bind call. Program ending.");
        return(SOCKET_ERR);
    }
    else
    {
    	// Log successful bind message.
        logger("Socket bind successful.");
    }

    // Set the listener socket as passive listener with specified queue size (backlog).
    if( listen(listenersocket, LISTENER_QUEUE_SIZE) < 0)
    {
        // Log error message and exit.
        logger("Error on listen call. Program ending.");
        return(SOCKET_ERR);
    }
    else
    {
    	// Log successful listening message.
        sprintf(logbuff, "Socket listen successful. Now listening on port %d.", port);
        logger(logbuff);
    }

    // Build the thread pool
    pool = threadpool_build();

    // Main listener loop.  An infinite loop that accepts connections on the listener socket
    // and creates a handler socket for each accepted connection.  The handler socket is
    // passed to a handler function.
    for(count = 1; ; count++)
    {
        length = sizeof(client_addr);

        // Accept a connection from the listener and create a new socket for it.
        if((handlersocket = accept(listenersocket, (struct sockaddr *) &client_addr, &length)) < 0)
        {
        	// Log error message.  Do not exit.  Loop back to get next connection.
            logger("Error on accept call.");
        }
        else
        {
        	// Log connection count.
            sprintf(logbuff, "*** Connection %d accepted. ***", count);
            logger(logbuff);

            // Add the valid connection to the thread pool queue
            add_connection(pool, handlersocket);
        }

    }
    
	threadpool_eliminate(pool);
    return(0);
}
