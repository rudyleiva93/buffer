// Daniel Hernandez
// Rudy Leiva
// 3/18/16
// CSC 341
// Project 3

/* Project 3: This program is passed command line arguments at the time of execution. Based on these arguments, the program decides how many producer and consumer threads to create and also how long the threads will sleep. The producer will generate a random number and insert it into the buffer. The consumer will remove the next available number form the buffer. Eah time the producer produces or the consumer consumes, the state of the buffer is outputted. Up to this point, all this is already given. We now discuss the insert_item and remove_item functions and the use of counting semaphores and a mutex lock. The insert_item function will insert a value into the buffer at the index indicated by the variable insertPointer. A calculation is performed on insertPointer to determine the next slot (usually just increments by 1 until the size of the buffer is reached, then the next slot is considered to be slot 0). The state of the buffer is then printed out. The remove_item function will look at the value in the index indicated by the variable removePointer. If the value at the index is a -1, nothing will happen because that means the slot is currently empty and nothing should be consumed and the value of removePointer should not be re-calculated. If there is a value to consume, that value is removed and a -1 is added to that slot to indicate that it is an empty slot. Once the value is remvoed, the value of removePointer is recalculated (same way as in the insert_item function). The state of the buffer is then printed out. The counting semaphores and mutex lock are both used in the insert_item and remove_item functions. In both functions, we call sem_wait() and pthread_mutex_lock() at the beginning of the functions. At the end of both functions, we call sem_post() and pthread_mutex_unlock(). The functions sem_wait() and sem_post() were created to handle the counting semaphores. The functions for mutex locks are provided by pthread.h. In the insert_item function, we call sem_wait() and pass it the pointer of empty so that the value of empty is decremented by 1 to indicate that a slot is being used. We also call pthread_mutex_lock and pass it the pointer of mutex. At the end of the function we call sem_post and pass it the pointer of full to increment it by 1 to indicate there is something in the buffer that can be consumed. We also call pthread_mutex_unlock. For the remove_item function, in the beginning we call sem_wait with pointer to full so the the value of full is decremented by 1 to indicate that an item has been consumed. We also call pthread_mutex_lock and pass it the pointer of the mutex. At the end of the function we call sem_post and pass it the pointer of empty to increment it by 1 to indicate there is 1 more slot available to insert an item. We also call pthread_mutex_unlock. Finally, for the extra credit, we show the thread ID by calling pthread_self in the producer and consumer functions
*/

/* (continued) and also in the insert and remove item functions. For the system time, we make use of the time() and clock() functions from the time.h file. Time() returns the current time from the system, and clock() is a program clock that returns number of ticks since the program started running. Note: comments were not provided for the code that is given unless a modification was made to it.
*/

/**
 * Producer Consumer (page 253)
 * using counting semaphores as well as a mutex lock.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h> // required import to display time

#define TRUE 1
#define BUFFER_SIZE 5
#define MAX_SLEEP 5

#define MIN 0  //minimum value of an element in buffer
#define MAX 1000 //maximum value of an element in buffer

int buffer[BUFFER_SIZE];

pthread_mutex_t mutex;
sem_t empty;
sem_t full;
time_t currentTime; // system time
clock_t programTime; // number of ticks since program started

int insertPointer = 0, removePointer = 0;

void *producer(void *param);
void *consumer(void *param);

void getTime()
{
	currentTime = time(NULL); // get the system time (Day Month Date HH:MM:SS Year)
}

int myRand() {
   return rand() % (MAX - MIN + 1) + MIN;
}

int print_buffer(){
	int i;
	for (i=0; i< BUFFER_SIZE; i++)
		printf("slot %d: %d\n", i, buffer[i]);
	printf("\n");	
	return 0;
}

int init_buffer(){
	int i;
	for (i=0; i< BUFFER_SIZE; i++)
		buffer[i] = -1;
	return 0;
}

int insert_item(int item)
{
	sem_wait(&empty); // decrement semaphore (1 less slot to insert)
	pthread_mutex_lock(&mutex); // locks the mutex (acquire)
	
	int test = buffer[insertPointer]; // checks if slot already has value
	
	if(test == -1) // is the slot empty?
	{
		buffer[insertPointer] = item; // insert item at slot pointed by insertPointer
		insertPointer = (insertPointer + 1) % BUFFER_SIZE; // calculate next slot (wraps around)
		getTime(); // get the system time as date and time
		programTime = clock(); // get clock ticks since program started (elapsed)
		
		printf("Producer produced %d at time %s (Program clock (ticks): %ld) (Thread ID: %lu)\n", item, ctime(&currentTime), programTime, pthread_self()); // output information
		print_buffer(); // prints state of buffer
	}
		
	pthread_mutex_unlock(&mutex); // unlock the mutex (release)
	sem_post(&full); // increment semaphroe (1 more item to consume)
}

int remove_item()
{
	sem_wait(&full); // decrement semaphore (1 less item to consoume)
	pthread_mutex_lock(&mutex); // lock the mutex (acquire)
	
	int item = buffer[removePointer]; // consume the item pointed in the byffer by removePointer
	
	if(item != -1) // Is the slot already empty
	{
		buffer[removePointer] = -1; // keep it empty
		removePointer = (removePointer + 1) % BUFFER_SIZE; // calculates next slot (wraps around)
		getTime(); // get system time as date and time
		programTime = clock(); // get clock tics since program started (elapsed)
	
		printf("Consumer consumed %d at time %s (Program clock (ticks): %ld) (Thread ID: %lu)\n", item, ctime(&currentTime), programTime, pthread_self()); // output information
		print_buffer(); // prints state of buffer
	}
	
	pthread_mutex_unlock(&mutex); // unlock the mutex (release)
	sem_post(&empty); // increment the semaphore (1 more slot to insert)
}

int main(int argc, char *argv[])
{
	int sleepTime, producerThreads, consumerThreads;
	int i, j;

	if(argc != 4)
	{
		fprintf(stderr, "Useage: <sleep time> <producer threads> <consumer threads>\n");
		return -1;
	}

	sleepTime = atoi(argv[1]);
	producerThreads = atoi(argv[2]);
	consumerThreads = atoi(argv[3]);

	/* Initialize buffer to be -1 */
	init_buffer();

	/* Initialize the synchronization tools */
	printf("%d\n",pthread_mutex_init(&mutex, NULL));
	printf("%d\n",sem_init(&empty, 0, BUFFER_SIZE));
	printf("%d\n",sem_init(&full, 0, 0));
	srand(time(0));

	/* Create the producer and consumer threads */
	for(i = 0; i < producerThreads; i++)
	{
		pthread_t tid;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_create(&tid, &attr, producer, NULL);
	}

	for(j = 0; j < consumerThreads; j++)
	{
		pthread_t tid;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_create(&tid, &attr, consumer, NULL);
	}

	/* Sleep for user specified time */
	sleep(sleepTime);
	return 0;
}


void *producer(void *param)
{
	int random;
	int r;

	while(TRUE)
	{
		r = rand() % MAX_SLEEP;
		sleep(r);
		random = myRand();
		getTime();
		programTime = clock();

		printf("Producer tries to insert %d at time %s (Program clock (ticks): %ld) (Thread ID: %lu)\n", random, ctime(&currentTime), programTime, pthread_self()); // Modified to show date and time, clock ticks elapsed since program started, and the thread ID.
		if(insert_item(random))
			fprintf(stderr, "Error\n");
	}

}

void *consumer(void *param)
{
	int r;

	while(TRUE)
	{
		r = rand() % MAX_SLEEP;
		sleep(r);
		getTime();
		programTime = clock();
		
		printf("Consumer tries to consume at time %s (Program clock (ticks): %ld) (Thread ID: %lu)\n", ctime(&currentTime), programTime, pthread_self()); // Modified to show date and time, clock ticks elapsed since program started, and the thread ID.

		if(remove_item())
			fprintf(stderr, "Error Consuming\n");
	}
}
