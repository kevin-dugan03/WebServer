/*
 * headerfile.h
 *
 * The application header file.
 */

#ifndef HEADERFILE_H_
#define HEADERFILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFSIZE 8096 /* default buffer size */
#define LISTENER_QUEUE_SIZE 64 /* default listener queue size */
#define DEFAULT_CONFIG_FILE "config.txt" // default config file name
#define DEFAULT_PORT 5555 /* default port */
#define DEFAULT_DIR "c:/webserver/home" /* default directory */
#define CMD_LINE_ARG_MAX 4 /* maximum number of command line arguments */
#define CMD_LINE_ERR 2 /* command line error code */
#define DIR_ERR 3 // home directory error
#define SOCKET_ERR 3 /* socket error code */
#define MAX_GET_REQUEST_SIZE 10000000 /* max size of a file that can be returned from a GET */
#define NV_DELIMITER '=' // name/value pair delimiter
#define ET_DELIMITER '&' // extension/type delimiter
#define FILETYPES_ARRAY_SIZE 100 // max elements in filetypes array
#define PORT_MIN 2000 // minimum allowed port number
#define PORT_MAX 65535 // maximum allowed port number
#define CONFIG_FILE_ERR 9 // configuration file not found or error
#define LOGFILE "/server.log" // log file name
#define DEFAULT_START "index.html"	//default page to open if none provided
#define MAX_THREADS 5	// number of threads to start in thread pool
#define QUEUE_SIZE  20  //number of waiting connections allowed in the queue

typedef struct threadpool threadpool;

// Function prototypes
// Listens for connections
int listener(int);

// Routes requests to appropriate handlers
void *router(void *);

// Processes GET requests
void processGet(int, char *);

// Processes HEAD requests
void processHead(int, char *);

// Processes POST requests
void processPost(int, char *);

// Gets the current date and time
void getTimestamp2(char *);

// Processes HTTP error codes
void sendError(int, int);

// Logs the transactions
void logger(char *);

// Read and process the configuration file
int readConfigFile(char *, char *, char *);

// Build a threadpool
threadpool *threadpool_build();

// Add a connection to the threadpool
int add_connection(threadpool *, int);

// Destroy the threadpool upon program exit
void threadpool_eliminate();

// Define type of struct for file types
typedef struct filetypes_template {
	int index;
    char extension[80];
    char type[80];
	} filetypes_template;

// Declare global array for file types
extern filetypes_template filetypes[];

// Global variable for log file path and name
extern char logfilePathAndName[];

#endif /* HEADERFILE_H_ */
