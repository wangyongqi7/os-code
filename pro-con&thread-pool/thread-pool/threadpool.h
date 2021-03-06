// function prototypes

typedef struct
{
    void (*function)(void* p);
    void* data;
} Task;
struct node
{
    Task task;
    struct node* next;
};

void execute(void (*somefunction)(void* p), void* p);
int pool_submit(void (*somefunction)(void* p), void* p);
void* worker(void* param);
void pool_init(void);
void pool_shutdown(void);
