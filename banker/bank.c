#include "stdio.h"
#include "unistd.h"
#include <stdlib.h>
#include <string.h>
#define NUMBER_OF_CONSUMERS 5
#define NUMBER_OF_RESOURCES 4

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CONSUMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CONSUMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CONSUMERS][NUMBER_OF_RESOURCES];

int test( int row ,int rq[]);     //test safety

void print();
void initial();
int request(int row,int rq[]);
int release(int row,int rl[]);
int clear();    //clear ok work
int main(int argc, char** argv)
{
    for (int i = 0; i < NUMBER_OF_RESOURCES;i++)
        available[i] = atoi(argv[i+1]);
    int run = 1;
    char a[30] = "maximum.txt";
    char task[100];
    char* temp;
    
    FILE* in = fopen(a, "r");
    int i = 0;

    while (fgets(task, 100, in) != NULL)
    {
        
        temp = strdup(task);
        maximum[i][0] = atoi(strsep(&temp, " "));
        maximum[i][1] = atoi(strsep(&temp, " "));
        maximum[i][2] = atoi(strsep(&temp, " "));
        maximum[i][3] = atoi(strsep(&temp, " "));
        i++;
    }
    fclose(in);
    initial();
    //print();
    while(run){
        clear();
        scanf("%s", task);
        if (strcmp(task, "*") == 0)
        {
            print();
        }
        else if(strcmp(task,"RQ")==0)
        {
            scanf("%d", &i);

            if(i>=NUMBER_OF_CONSUMERS)
            {
                printf("no this consumer \n");
                continue;
            }
            
            int Request[NUMBER_OF_RESOURCES];
            memset(Request, 0, sizeof(Request));
            for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
            {
                scanf("%d", &Request[j]);
            }
            if(test(i,Request)==0)
                request(i,Request);
             else
                printf("a unsafe request is rejected \n");
        }
        else if (strcmp(task,"RL")==0)
        {
            scanf("%d", &i);
            if(i>=NUMBER_OF_CONSUMERS)
            {
                printf("no this consumer \n");
                continue;
            }
            int Release[NUMBER_OF_RESOURCES];
            memset(Release, 0, sizeof(Release));
            for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
            {
                scanf("%d", &Release[j]);
            }
            release(i, Release);
        }
        else if(strcmp(task,"end")==0)
        {
            run = 0;
        }
        else
            printf("wrong command\n");
    }

    return 0;
}

void print()
{
    printf("maximum array is \n");
    for (int i = 0; i < NUMBER_OF_CONSUMERS; i++)
    {
        for (int j = 0; j < NUMBER_OF_RESOURCES;j++)
            printf("%d\t", maximum[i][j]);
        printf("\n");
    }
    printf("need array is \n");
    for (int i = 0; i < NUMBER_OF_CONSUMERS; i++)
    {
        for (int j = 0; j < NUMBER_OF_RESOURCES;j++)
            printf("%d\t", need[i][j]);
        printf("\n");
    }
    printf("allocation array is \n");
     for (int i = 0; i < NUMBER_OF_CONSUMERS; i++)
    {
        for (int j = 0; j < NUMBER_OF_RESOURCES;j++)
            printf("%d\t", allocation[i][j]);
        printf("\n");
    }
    printf("available array is \n");
    for (int i = 0; i < NUMBER_OF_RESOURCES;i++)
        printf("%d\t", available[i]);
    printf("\n");

}

int clear()
{
    for (int i = 0; i < NUMBER_OF_CONSUMERS;i++)
    {
        int flagclear = 0;
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
            if (need [i][j]>0)
                flagclear = 1;

        if(flagclear==0)
            for(int j=0;j<NUMBER_OF_RESOURCES;j++)
            {
                available[j] += allocation[i][j];
                allocation[i][j] = 0;
            }
    }
    
}

void initial()
{
    //memset(available, 0, sizeof(available));
    memset(need, 0, sizeof(need));
    for (int i = 0; i < NUMBER_OF_CONSUMERS;i++)
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
            need[i][j] = maximum[i][j]-allocation[i][j];
    return;
}

int test(int row ,int rq[])
{
    int inflag = 0;
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        if(rq[i]>need[row][i])
            inflag = 1;
        if(rq[i]<0)
            inflag = 2;
    }

    if(inflag!=0)
    {
        if(inflag==1)
            printf("request is large than need\n");
        if(inflag==2)
            printf("request have negative value!!\n");
        return 1;
    }
    int temp[NUMBER_OF_RESOURCES];
    int tempneed[NUMBER_OF_CONSUMERS][NUMBER_OF_RESOURCES];
    
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        temp[i] = available[i] - rq[i];
    
    for (int i = 0; i < NUMBER_OF_CONSUMERS;i++)
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
            tempneed[i][j] = need[i][j];

     for (int i = 0; i < NUMBER_OF_RESOURCES;i++)
        tempneed[row][i] -= rq[i];
        
    int bool_c[NUMBER_OF_CONSUMERS];
    memset(bool_c, 0, sizeof(bool_c));

    int size = 0;
 

    while (size<NUMBER_OF_CONSUMERS)
    {
         for (int i = 0; i < NUMBER_OF_CONSUMERS;i++)
         {
             if(bool_c[i]==1)
                 continue;
             int flag = 0;
             for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
                 if (temp[j] <tempneed[i][j])
                     flag = 1;
            if(flag==0)
            {
                bool_c[i] = 1;
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
                temp[j] += allocation[i][j];
                if(i==row)
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
                temp[j] += rq[j];
                break; 
            }
         }
            size++;
    }
     int x = 0;
    for (int i = 0; i < NUMBER_OF_CONSUMERS;i++)
    if(bool_c[i]==0)
        x = 1;
    return x;
}
int request(int row, int rq[])
{
    for (int i = 0; i < NUMBER_OF_RESOURCES;i++)
    {
        allocation[row][i] = allocation[row][i] + rq[i];
        need[row][i] = maximum[row][i] - allocation[row][i];
        available[i] -= rq[i];
    }

    return 0;
}
int release(int row, int rl[])

{
    for (int i = 0; i < NUMBER_OF_RESOURCES;i++)
    {
        allocation[row][i] = allocation[row][i] - rl[i];
        if(allocation[row][i]<0)
        {
            printf("consumer %d donont have so much resource %d", row, i);
            allocation[row][i] = 0;
        }
        need[row][i] = maximum[row][i] - allocation[row][i];
        available[i] += rl[i];
    }
        return 0;
}
