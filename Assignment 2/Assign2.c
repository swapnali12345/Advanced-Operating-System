/*
Name: Swapnali Wakchaure
ID: 700703307
Assignment 2 : C program to utilize multi-threads,
mutex lock, and semaphores for the producer and consumer problem
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 32//Implementing a circular queue of size 32
#define PROD_THREADS 2 /*Producers Threads No 0,2 (even)*/
#define CONS_THREADS 1 /*Consumer Threads No 1 (odd)*/
#define PROG_EXEC_TIME 100000000 /* Indefinite can be changed to MAXLIMIT program run time in Sec*/
typedef int bufferItem;

bufferItem buffer[BUFFER_SIZE+1];
int head = 0; /* beginning of queue buffer */
int rear = 0; /* end of queue buffer */
int buffercount = 0; /* count of queue buffer elements */

pthread_mutex_t mutex;
sem_t empty; /* semaphore */
sem_t full; /* semaphore */

/***********************************************************************
* Adds item to front of the circular queue buffer
************************************************************************/
int enqueue(bufferItem item)
{
    if (head == (rear + 1) % (BUFFER_SIZE + 1)) return 1;
    buffer[rear++] = item;
    buffercount++;
    if (rear == BUFFER_SIZE + 1) rear = 0;
    return 0;

}

/***********************************************************************
*Removes the first element from the circular queue buffer FIFO order
************************************************************************/
int dequeue(bufferItem *item)
{

    if (head == rear) return 1;
    *item = buffer[head++];
    buffercount--;
    if (head == BUFFER_SIZE + 1) head = 0;
    return 0;

}

/***********************************************************************
* Producer thread waits a random amount of time before attempting to produce
* an item and store it in the buffer and Repeats
************************************************************************/
void *producer(void *param)
{
   bufferItem itemProduced;
   int *id = (int*)param;

   while (1) /* run until cancelled */
   {
      /* sleep for a random 0-2 seconds period of time */
      sleep(rand() % 2);
      /* generate a random number from 0 to 10 */
      itemProduced = rand() % 10; /* range is 0-10 */

      /* check if the buffer is full*/
      if(buffercount>=BUFFER_SIZE)
      {
       printf("BUFFER is FULL ...\n");
      }
      /* the consumer will call signal(empty) */
      sem_wait(&empty);
      pthread_mutex_lock(&mutex); /*mutex lock*/
      /* START: Critical section */
      int bufout = enqueue(itemProduced);
      if (bufout)
         printf("BUFFER is FULL ...\n");
      else
        printf("Thread %d - Produce %d (%d)\n", *id, itemProduced,buffercount);
      /* END: Critical section */
      pthread_mutex_unlock(&mutex);
      sem_post(&full); /* the consumer called wait(full) */

   }
}

/***********************************************************************
* Consumer Thread waits a random amount of time before attempting to consume
* an item from the buffer and Repeats
************************************************************************/
void *consumer(void *param)
{
   bufferItem consumedItem;
   int *id = (int*)param;

   while (1) /* run until cancelled */
   {
      /* sleep for a random 0-2 seconds period of time */
      sleep(rand() % 2);
      /* check if the buffer is EMPTY*/
      if(buffercount<=0)
      {
       printf("BUFFER is EMPTY ...\n");
      }
      /* the producer will call signal(full) */
      sem_wait(&full);
      pthread_mutex_lock(&mutex); /*mutex lock*/
      /* START: Critical section */
      if (dequeue(&consumedItem))
          printf("BUFFER is EMPTY ...\n");
      else
         printf("Thread %d - Consume %d (%d)\n", *id, consumedItem,buffercount);
      /* END: Critical section */

      pthread_mutex_unlock(&mutex);
      sem_post(&empty); /* the producer called wait(empty) */
   }
}

/***********************************************************************
* Attempts to initialize mutex and semaphore variables
* Program exits if unsuccessful
************************************************************************/
void initializeMutexAndSemaphores()
{
   int s;

   /* Initialize 'mutex' mutex */
   s = pthread_mutex_init(&mutex, NULL);
   if (s != 0)
   {
      printf("Couldn't initialize 'mutex' mutex\n");
      exit(-1);
   }

   /* Initialize 'empty' semaphore */
   s = sem_init(&empty, 0, BUFFER_SIZE);
   if (s != 0)
   {
      printf("Couldn't initialize 'empty' semaphore\n");
      exit(-1);
   }

   /* Initialize 'full' semaphore */
   s = sem_init(&full, 0, 0);
   if (s != 0)
   {
      printf("Couldn't initialize 'full' semaphore\n");
      exit(-1);
   }
}

/***********************************************************************
* Attempts to create producer and consumer thread arrays
* Program exits if unsuccessful
************************************************************************/
void createThreads(pthread_t **pThreads, int pNum,
                   pthread_t **cThreads, int cNum)
{
   int i;
   int s;

   /* Create producer threads */
   *pThreads = (pthread_t*)malloc(pNum * sizeof (pthread_t));
   if (pThreads == NULL)
   {
      fprintf(stderr, "malloc error\n");
      exit(-1);
   }
   for (i = 0; i <= pNum; i++)
   {
      int *id = (int*)malloc(sizeof (int));
      *id = i; /* Producer id (Not related to process id) */
      //printf("i is %d",i);
      s = pthread_create(&(*pThreads)[i], NULL, producer, id);

      /* Check to see if error in thread creation. Exit if an error. */
      if (s != 0)
      {
         fprintf(stderr, "Couldn't create thread\n");
         exit(-1);
      }
      i= i+1;
   }

   /* Create consumer threads */
   *cThreads = (pthread_t*)malloc(cNum * sizeof (pthread_t));
   if (cThreads == NULL)
   {
      fprintf(stderr, "malloc error\n");
      exit(-1);
   }

   for (i = 0; i < cNum; i++)
   {
      int *id = (int*)malloc(sizeof (int));
      *id = i + 1;
      s = pthread_create(&(*cThreads)[i], NULL, consumer, id);

      if (s != 0)
      {
         fprintf(stderr, "Couldn't create thread\n");
         exit(-1);
      }

   }

}

/***********************************************************************
* Attempts to cancel all consumer and producer threads
* Program exits if unsuccessful
************************************************************************/
void cancelThreads(pthread_t **pThreads, int pNum,
                   pthread_t **cThreads, int cNum)
{
   int i;

   /* Cancel all producer threads */
   for (i = 0; i <= pNum; i++)
   {
      int s = pthread_cancel((*pThreads)[i]);
      if (s != 0)
      {
         fprintf(stderr, "Couldn't cancel thread\n");
         exit(-1);
      }
      i= i+1;
   }

   /* Cancel all consumer threads */
   for (i = 0; i < cNum; i++)
   {
      int s = pthread_cancel((*cThreads)[i]);
      if (s != 0)
      {
         fprintf(stderr, "Couldn't cancel thread\n");
         exit(-1);
      }
   }
}

/***********************************************************************
* Main Driver code
* Initialize mutex and semaphore variables
* Creates producer and consumer threads
* Sleep Cancel threads and Exits
************************************************************************/
int main(int argc, char **argv)
{
   pthread_t *producers;
   pthread_t *consumers;

   int sleepTime = PROG_EXEC_TIME ;
   int nProducers = PROD_THREADS ; /* number of producers Threads */
   int nConsumers = CONS_THREADS ; /* number of consumer Threads */

   /* Initialize buffer */
   memset(buffer, 0, BUFFER_SIZE * sizeof (bufferItem));

   /* Initialize the mutex lock and semaphores */
   initializeMutexAndSemaphores();

   printf("Producer and Consumer program Starting \n");
   printf("=======================================\n");

   /* Create producer threads(s) */
   /* Create consumer threads(s) */
   createThreads(&producers, nProducers, &consumers, nConsumers);

   /* Sleep Time */
   sleep(sleepTime);

   /* Cancel threads */
   cancelThreads(&producers, nProducers, &consumers, nConsumers);

   /* Exit */
   free(producers); /* clean up */
   free(consumers);
   return 0;
}
