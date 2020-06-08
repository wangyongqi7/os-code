/**
 * Implementation of thread pool.
 */

#include "threadpool.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>


#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool

struct node *head, *tmp;

Task t;
int shutdown;
// the worker bee
pthread_t bee[NUMBER_OF_THREADS];
pthread_mutex_t mux;
sem_t sem;
int numOfTask;
// insert a task into the queue
// returns 0 if successful or 1 otherwise,
int enqueue(Task t) 
{
    struct  node* p;
    p = malloc(sizeof(struct node));
    tmp = head;
    p->task = t;
    p->next = tmp;
    head = p;
    
    return 0;
}

// remove a task from the queue
Task dequeue() 
{

        tmp = head;
        t = head->task;
        head = head->next;
        free(tmp);
   
    return t;
}

void execute(void (*somefunction)(void *p), void *p)
{
    printf("executing\n");
    (*somefunction)(p);
    printf("finish execution\n");
}

// the worker thread in the thread pool
void *worker(void *param)
{
    // execute the task
    
    while (1)
    {
        
            while(numOfTask==0&&shutdown==0){
                printf("waiting for a task\n");
                sem_wait(&sem);
            }

        if(shutdown)
        {
            printf("the thread pool is shutdowning \n");
            pthread_exit(0);
        }

        pthread_mutex_lock(&mux);
        t = dequeue();
        numOfTask--;
        pthread_mutex_unlock(&mux);
        
        execute(t.function, t.data);

    }
    printf("the thread pool is shutdowning \n");
    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{

    t.function = somefunction;
    t.data = p;
    pthread_mutex_lock(&mux);
    enqueue(t);
    numOfTask++;
    sem_post(&sem);
    pthread_mutex_unlock(&mux);
    printf("submit a new work");
    return 0;
}

// initialize the thread pool
void pool_init(void)
{
    pthread_mutex_init(&mux, NULL);
    sem_init(&sem, 0, 0);
    numOfTask = 0;
    shutdown = 0;
    head = tmp = NULL;
    for (int i = 0; i < NUMBER_OF_THREADS;i++)
        pthread_create(&bee[i], NULL, worker, NULL);
}

// shutdown the thread pool
void pool_shutdown(void)
{
    pthread_mutex_lock(&mux);
    shutdown = 1;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp);
    }
    for (int i = 0; i < NUMBER_OF_THREADS;i++)
    {
        sem_post(&sem);
    }
        pthread_mutex_unlock(&mux);
        printf("out mutex\n");
        
        for (int i = 0; i < NUMBER_OF_THREADS; i++)
            pthread_join(bee[i], NULL);
        pthread_mutex_destroy(&mux);
        sem_destroy(&sem);
}
