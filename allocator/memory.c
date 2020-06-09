#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "stdlib.h"

struct node 
{
    int id;
    int relocate;
    int limit;
    struct node* next;
};

struct node* head;

int initial(int size);
int request(int id,int rqSize,int type);
int release(int id);
void print();
void merge();
void mergeFrag();
struct node *findHole(int size, int type, struct node* target);

int main(int args, char** argv)
{
    int size;
    int run = 1;
    if (argv[1] != NULL)
        size = atoi(argv[1]);
    else
    {
        printf("input the maximum size of memory\n");
        return 0;
    }
    
    initial(size);
    printf("size = %d\n", size);
    
    if(head==NULL)
        printf("initial error\n");
    char comm[100];
    while (run)
    {
        merge();
        printf("allocator>");
        scanf("%s", comm);
       
        if (strcmp(comm, "RQ") == 0)
        {
            int id;
            char typestr[10];
            char idstr[10];
            scanf("%s%d%s", idstr, &size, typestr);
            id = atoi(idstr + 1);
            switch (typestr[0])
            {
                case 'F': 
                {
                    request(id, size, 0);
                    break;
                }
                case 'B':
                {
                    request(id, size, 1);
                    break;
                }
                
                case 'W':
                {
                    request(id, size, 2);
                    break;
                }
                default: {
                    printf("%s wrong fit command\n",typestr);
                }
            }
        }
        else if (strcmp(comm, "RL") == 0)
        {
            int id;
            char idstr[10];
            scanf("%s", idstr);
            id = atoi(idstr + 1);
            release(id);
        }
        else if (strcmp(comm, "STAT") == 0)
        {
             //return 0;printf("in ok\n");
            
            print();
           // printf("out ok\n");
        }
        else if (strcmp(comm, "C") == 0)
        {
            mergeFrag();
        }
        else if (strcmp(comm, "X") == 0)
        {
            run = 0;
        }
            else
            {
                printf("wrong command \n");
                continue;
            }
    }

    return 0;
}

int initial(int size)
{
    head = malloc(sizeof(struct node));
    head->id = -1;
    head->relocate = 0;
    head->limit = size;
    head->next = NULL;
    return 0;
}

int request(int id,int rqSize,int type)
{
    struct node* target;
    
    target=findHole(rqSize, type, target);
    if(target!=NULL)
    {
        struct node* newhole;
        newhole = malloc(sizeof(struct node));
        newhole->limit = target->limit - rqSize;
        newhole->id = -1;
        newhole->relocate = target->relocate + rqSize;
        newhole->next = target->next;
        target->limit = rqSize;
        target->id = id;
        target->next = newhole;
    }
    else 
    {
        printf("no available memory space for this process\n");
        return 1;
    }
    return 0;
}

int release(int id)
{
    struct node* tmp;
    tmp = head;
    while(tmp!=NULL)
    {
        if(tmp->id==id)
            {
                tmp->id = -1;
                break;
            }
            tmp = tmp->next;
    }
}

void print()
{
    //return;
    struct node* tmp;
    tmp = head;
    while(tmp!=NULL)
    {
        if(tmp->id==-1)
        {
            printf("%d\t%d\t--unused\n", tmp->relocate,
                   tmp->relocate + tmp->limit-1);
        }
        else {
            printf("%d\t%d\t--P%d\n", tmp->relocate, tmp->relocate + tmp->limit-1,
                   tmp->id);
        }
        tmp = tmp->next;
    }
}

void merge()
{
    struct node* tmp;
    tmp = head;
    while (tmp!=NULL)
    {
        if(tmp->id==-1&&tmp->next!=NULL&&tmp->next->id==-1)
        {
            tmp->limit += tmp->next->limit;
            struct node* m = tmp->next;
            tmp->next = m->next;
            free(m);
        }
        else
            tmp = tmp->next;
    }
}

void mergeFrag()
{
    struct node* tmp = head;
    while (tmp!=NULL)
    {
        if(tmp->next!=NULL)
        {
            if(tmp->next->id==-1)
            {
                struct node* tmp1=tmp->next;
                //struct node* tmp2 = head;
                tmp->next = tmp->next->next;
                tmp1->next = head;
                head = tmp1;
            }
            else
                tmp = tmp->next;
        }
        else
            tmp = tmp->next;
    }
    head->relocate = 0;
    tmp = head;
    while (tmp->next != NULL)
    {
        tmp->next->relocate = tmp->relocate + tmp->limit;
        tmp = tmp->next;
    }
    return;
}

struct node * findHole(int size, int type, struct node* target)
{
    struct node *tar;
    if (type == 0)      // first fit
    {
        
        struct node* tmp = head;

        while (tmp != NULL)
        {
            if (tmp->id == -1 && tmp->limit >= size)
            {

                tar = tmp;
                break;
            }
            tmp = tmp->next;
        }
    }

    if(type==1)         //best fit
    {
        struct node* tmp = head;
        while (tmp != NULL)
        {
            if (tmp->id == -1 && tmp->limit >= size)
            {
                if (tar == NULL)
                    tar = tmp;
                 else if(tar->limit>tmp->limit)
                    tar = tmp;
            }
            tmp = tmp->next;
        }
    }

    if(type==2)         //worst fit
    {
        struct node* tmp = head;
        while (tmp != NULL)
        {
            if (tmp->id == -1 && tmp->limit >= size)
            {
                if (tar == NULL)
                    tar = tmp;
                else if(tar->limit<tmp->limit)
                    tar = tmp;
            }
            tmp = tmp->next;
        }
    }
    return tar;
}