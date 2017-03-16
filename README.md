# buffer

Team members: Daniel Hernandez, Rudy Leiva

How to compile:

Linux/UNIX: Open terminal, cd to the directory where the source code file is stored. Run "gcc -o buffer buffer.c -pthread" (without quotes) on the terminal window. To run the program, simply do "./buffer <sleep time> <producer thread> <consumer threads>" (without quotes) (replace each <> with actual integer numbers). Example: ./buffer 20 5 5

Windows & Mac OS X: NOT COMPATIBLE - pthread(), semaphore() and mutex() functions only available on Linux/UNIX. Semaphore() functions used not portable to Mac OS X.

Files included:

buffer.c (Source code)
buffer (C executable file)
output2.txt (Text file containing output of program - included for easy readability of output)

Description / purpose of program:

Project 3: This program is passed command line arguments at the time of execution. Based on these arguments, the program decides how many producer and consumer threads to create and also how long the threads will sleep. The producer will generate a random number and insert it into the buffer. The consumer will remove the next available number form the buffer. Eah time the producer produces or the consumer consumes, the state of the buffer is outputted. Up to this point, all this is already given. We now discuss the insert_item and remove_item functions and the use of counting semaphores and a mutex lock. The insert_item function will insert a value into the buffer at the index indicated by the variable insertPointer. A calculation is performed on insertPointer to determine the next slot (usually just increments by 1 until the size of the buffer is reached, then the next slot is considered to be slot 0). The state of the buffer is then printed out. The remove_item function will look at the value in the index indicated by the variable removePointer. If the value at the index is a -1, nothing will happen because that means the slot is currently empty and nothing should be consumed and the value of removePointer should not be re-calculated. If there is a value to consume, that value is removed and a -1 is added to that slot to indicate that it is an empty slot. Once the value is remvoed, the value of removePointer is recalculated (same way as in the insert_item function). The state of the buffer is then printed out. The counting semaphores and mutex lock are both used in the insert_item and remove_item functions. In both functions, we call sem_wait() and pthread_mutex_lock() at the beginning of the functions. At the end of both functions, we call sem_post() and pthread_mutex_unlock(). The functions sem_wait() and sem_post() were created to handle the counting semaphores. The functions for mutex locks are provided by pthread.h. In the insert_item function, we call sem_wait() and pass it the pointer of empty so that the value of empty is decremented by 1 to indicate that a slot is being used. We also call pthread_mutex_lock and pass it the pointer of mutex. At the end of the function we call sem_post and pass it the pointer of full to increment it by 1 to indicate there is something in the buffer that can be consumed. We also call pthread_mutex_unlock. For the remove_item function, in the beginning we call sem_wait with pointer to full so the the value of full is decremented by 1 to indicate that an item has been consumed. We also call pthread_mutex_lock and pass it the pointer of the mutex. At the end of the function we call sem_post and pass it the pointer of empty to increment it by 1 to indicate there is 1 more slot available to insert an item. We also call pthread_mutex_unlock. Finally, for the extra credit, we show the thread ID by calling pthread_self in the producer and consumer functions and also in the insert and remove item functions. For the system time, we make use of the time() and clock() functions from the time.h file. Time() returns the current time from the system, and clock() is a program clock that returns number of ticks since the program started running. Note: comments were not provided for the code that is given unless a modification was made to it.

Team member contributions:

Daniel Hernandez -

code development
code debugging
semaphore research
implement semaphores
Thread ID implementation
CLock ticks implementation
insert_item function
error checking with buffer
buffer.c
README

Rudy Leiva -

code development
code debugging
mutex lock research
system time research (functions in time.h)
implement mutex lock
System time (as date and time) implementation
remove_item function
error checking with buffer
buffer.c
