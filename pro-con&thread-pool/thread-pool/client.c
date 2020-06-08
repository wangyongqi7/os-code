/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    // create some work to do
    struct data work;
    work.a = 5;
    work.b = 10;

    struct data work1;
    work1.a = 11;
    work1.b = 19;

    struct data work2;
    work2.a = 3;
    work2.b = 1;

    struct data work3;
    work3.a = 3;
    work3.b = 11;

    // initialize the thread pool
    pool_init();
    sleep(1);
    // submit the work to the queue
    pool_submit(&add,&work);
    pool_submit(&add,&work1);
    pool_submit(&add,&work2);
    pool_submit(&add,&work3);
    // may be helpful 
    sleep(3);
    
    pool_shutdown();

    return 0;
}
