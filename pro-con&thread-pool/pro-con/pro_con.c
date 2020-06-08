#include "stdio.h"
#include  "pthread.h"
#include "semaphore.h"
#include "stdlib.h"
#include "time.h"
#include "unistd.h"

#define MAX_BUFFER_SIZE  10
#define MAX_PRODUCE_SIZE  5
#define MAX_CONSUMER_SIZE  5

typedef int buffer_item;

sem_t full, empty, mux;
int head,tail;
buffer_item buffer[MAX_BUFFER_SIZE];

void initial()
{
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, MAX_BUFFER_SIZE - 1);
    sem_init(&mux, 0, 1);
    tail = 0;
    head = 0;
    srand(time(NULL));
}
int insert_item(buffer_item item)
{
    sem_wait(&empty);
    sem_wait(&mux);
    buffer[tail] = item;
    tail++;
    tail = tail % MAX_BUFFER_SIZE;

    sem_post(&full);
    sem_post(&mux);
    return 0;
}
int remove_item(buffer_item *item)
{
    sem_wait(&full);
    sem_wait(&mux);
    *item = buffer[head];
    head++;
    head = head % MAX_BUFFER_SIZE;
    sem_post(&empty);
    sem_post(&mux);
    return 0;
}


int test(int pnum,int cnum,int sleeptime)
{
    if(pnum>MAX_PRODUCE_SIZE||cnum>MAX_CONSUMER_SIZE||sleeptime>1000)
    {
        printf("error input\n");
        return 1;
    }
    else
        return 0;
}
void *producer(void *argv)
{
    int id = *(int*)argv;
    buffer_item item;
    while (1)
    {
        item = rand() % 100;
        int sleeptime = rand() % 3;
        
        insert_item(item);
        printf("producer %d produce number %d \n", id, item);
        sleep(sleeptime);
    }
}
void *consumer(void *argv)
{
    int id = *(int*)argv;
    buffer_item item ;
    while(1)
    {
        int sleeptime = rand() % 3;
        
        remove_item(&item);
        printf("consumer %d consumes %d\t\n", id, item);
        sleep(sleeptime);
    }
}
int main ( int argc,char **argv)
{
    int pnum, cnum, sleeptime;
    pnum = cnum = sleeptime = 0;
    

    sleeptime=atoi(argv[1]);
	pnum=atoi(argv[2]);
	cnum=atoi(argv[3]);

    if(test(pnum,cnum,sleeptime)==1)
        return 0;
    else
        printf("sleeptime is %d   pnum is %d     cnum is %d    \n", sleeptime,
               pnum, cnum);

    initial();
    pthread_t ptid[pnum];
    pthread_t ctid[cnum];
    
    for (int i = 0; i < pnum;i++)
        {
            pthread_create(&ptid[i], NULL, producer, &i);
            usleep(100);
        }
    for (int i = 0; i < cnum; i++)
        {
            pthread_create(&ctid[i], NULL, consumer, &i);
            usleep(100);
        }

    sleep(sleeptime);
    //end all threads
    for (int i = 0; i < pnum;i++)
    {
		pthread_cancel(ptid[i]);
		pthread_join(ptid[i], NULL);	
	}
    for (int i = 0; i < cnum; i++)
	{
		pthread_cancel(ctid[i]);
        pthread_join(ctid[i], NULL);
    }
    sem_destroy(&full);
    sem_destroy(&empty);
    sem_destroy(&mux);
}