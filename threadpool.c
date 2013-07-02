/*
 * threadpool.c
 *
 * Contains the functions to handle the thread pool for
 * multi-threading capability in the web server. The functions
 * include building the threadpool, adding connections to the
 * queue that need to be processed, and destroying the thread
 * pool when the program ends.
 *
 * Kevin Dugan
 * 11/23/2012
 */
#include "headerfile.h"

/*
 * Struct that holds the mutual exclusion lock, threads, and queue for
 * the threadpool.
 */
struct threadpool {
	pthread_mutex_t thread_lock;
	pthread_cond_t signal;
	pthread_t *threads;
	int *connection_queue;
	int queue_head;
	int queue_tail;
	int connection_count;
};

/*
 * Function prototypes for the threadpool.c file
 */
static void *worker_thread(void *t_pool);

void threadpool_deallocate(threadpool *t_pool);

/*
 * Function: threadpool_build
 * ----------------------------
 *   Builds the threadpool using the global variables
 *   for the max number of threads and the queue size.
 *
 *	 Parameters:
 *   none
 *
 *   Returns: the threadpool
 */
threadpool *threadpool_build()
{
	threadpool *pool;	//the threadpool
	int i;	//loop variable
	int valid_thread = 0;	//flag to check that a thread was created properly

	// Allocate memory
	pool = (threadpool *)malloc(sizeof(threadpool));
	pool->threads = (pthread_t *) malloc(sizeof(pthread_t) * MAX_THREADS);
	pool->connection_queue = (int *) malloc(sizeof(int) * QUEUE_SIZE);

	// Initialize components
	pool->queue_head = 0;
	pool->queue_tail = 0;
	pool->connection_count = 0;
	pthread_mutex_init(&(pool->thread_lock), NULL);
	pthread_cond_init(&(pool->signal), NULL);

	// Build the threads
	for (i = 0; i < MAX_THREADS; i++)
	{
		// Create the new thread, send into waiting status
		valid_thread = pthread_create(&(pool->threads[i]), NULL, worker_thread, (void*) pool);
		if (valid_thread != 0)
		{
			//threadpool_eliminate(pool);
			return NULL;
		}
	}

	// Return the complete thread pool
	return pool;
}

/*
 * Function: worker_thread
 * ----------------------------
 *   Readies the worker thread to process a socket connection.
 *
 *	 Parameters:
 *   t_pool: The threadpool
 *
 *   Returns: 0 if successful
 */
static void *worker_thread(void *t_pool)
{
	threadpool *pool = (threadpool *) t_pool;
	int connection;
	char logbuff[200];

	sprintf(logbuff, "Thread %u started", (unsigned int) pthread_self());
	logger(logbuff);

	for(;;)
	{
		//pthread_mutex_lock(&(pool->thread_lock));
		while (pool->connection_count == 0)
		{
			sprintf(logbuff, "Thread %u in wait status", (unsigned int) pthread_self());
			logger(logbuff);
			pthread_cond_wait(&(pool->signal), &(pool->thread_lock));
		}

		// Get the first connection from the front of the queue
		connection = pool->connection_queue[pool->queue_head];
		pool->queue_head += 1;	//move the head to the next item in the queue

		// If the head marker was just at the last item in the queue, send
		// it back to the front of the queue.
		if (pool->queue_head == QUEUE_SIZE)
		{
			pool->queue_head = 0;
		}

		pool->connection_count -= 1;	//subtract from the connections left to be processed
		//pthread_mutex_unlock(&(pool->thread_lock));

		// Send the connection to the router for processing
		(*(router))((void*)connection);

	}

	pthread_exit(NULL);
	return NULL;
}

/*
 * Function: add_connection
 * ----------------------------
 *   Adds a valid connection to the connection queue.
 *
 *	 Parameters:
 *   pool: The threadpool
 *   socketfd: The socket file descriptor for the connection
 *
 *   Returns: 0 if successful
 */
int add_connection(threadpool *pool, int socketfd)
{
	int result = 0;
	int next;

	// Increment the 'next' pointer
	next = pool->queue_tail + 1;

	// If the 'next' pointer is at the end of the queue, recycle to the
	//beginning of the queue.
	if (next == QUEUE_SIZE)
	{
		next = 0;
	}

	do
	{
		// Check that we can accept another connection
		if (pool->connection_count == QUEUE_SIZE)
		{
			logger("The queue is full");
			result = -1;
			break;
		}

		// Insert the connection into the end of the queue
		pool->connection_queue[pool->queue_tail] = socketfd;
		pool->queue_tail = next;
		pool->connection_count += 1;

		// Signal the thread pool that a connection is waiting
		pthread_cond_signal(&(pool->signal));

	}while(0);

	return result;
}

/*
 * Function: threadpool_eliminate
 * ----------------------------
 *   Destroys the thread pool upon program exit
 *
 *	 Parameters:
 *   None
 *
 *   Returns: nothing
 */
void threadpool_eliminate(threadpool *t_pool)
{
	threadpool *pool = t_pool;
	int i;

	logger("Joining threads to terminate all thread pool threads");
	for(i = 0; i < MAX_THREADS; i++)
	{
		pthread_join(pool->threads[i], NULL);
	}

	free(pool);
}
