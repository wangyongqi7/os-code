#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#define BACKING_STORE "BACKING_STORE.bin"

#define SIZE 256
#define PAGE_SIZE 8
#define PAGE_NUMBER 8
#define MAX_PAGE_NUMBER  256
#define FRAME_SIZE 8
#define FRAME_NUMBER 8
#define MAX_FRAME_NUMBER 128
#define MAX_TLB_NUMBER 16
#define MAX_IN_SIZE 1000


struct tlbunit
{
    int va; //virtual address
    int pa; //physical address
    int lru;    //lru number
};


int pagetable[MAX_PAGE_NUMBER];
char phyMem[SIZE][ MAX_FRAME_NUMBER];

int memory_lru[MAX_FRAME_NUMBER];
struct tlbunit TLB[MAX_TLB_NUMBER];

void initial();
int testTLB(int page_idx);
int updateTLBLRU(int page_idx,int frame_idx);
int updateFrameLRU(int frame_idx);
int searchPageTable(int page_index);
int findFreeFrame(int page_index);

int main(int argc, char** argv)
{
    if(argc != 2){
        printf("Invalid arguments!\n");
        return 0;
    }
    initial();
    FILE* fp = fopen(argv[1], "r");
    FILE *fp_back = fopen(BACKING_STORE, "rb");
    int address;
    int page_idx;
    int offset;
    int TLBhit = 0;
    int pagefault = 0;
    char result;

    while (!feof(fp))
    {
        fscanf(fp, "%d", &address);
        offset = address % SIZE;
        page_idx = (address / SIZE) % MAX_PAGE_NUMBER;

        if(testTLB(page_idx)!=-1)
        {
            TLBhit++;
            int index = testTLB(page_idx);
            int frame_idx = TLB[index].pa;
            updateTLBLRU(page_idx,frame_idx);
            updateFrameLRU(frame_idx);
            result = phyMem[frame_idx][offset];
            printf("logical address:%d ,physical address: %d ,result is %d "
                   "(TLB hit)\n",
                   address, SIZE * frame_idx + offset, result);
        }
        else if(searchPageTable(page_idx)!=-1)
        {
            int frame_idx = pagetable[page_idx];
            updateFrameLRU(frame_idx);
            updateTLBLRU(page_idx, frame_idx);
            result = phyMem[frame_idx][offset];
            printf("logical address:%d ,physical address: %d ,result is %d "
                   "(found)\n",
                   address, SIZE * frame_idx + offset, result);
        }
        else
        {
            pagefault++;
            char buffer[SIZE];
            fseek(fp_back, (page_idx)*PAGE_SIZE, SEEK_SET);
            fread(buffer, sizeof(char), PAGE_SIZE, fp_back);
            int frame_idx = findFreeFrame(page_idx);
            pagetable[page_idx] = frame_idx;
            memcpy(phyMem[frame_idx], buffer, SIZE);
            updateFrameLRU(frame_idx);
            updateTLBLRU(page_idx, frame_idx);

            result = phyMem[frame_idx][offset];
            printf("logical address:%d ,physical address: %d ,result is %d "
                   "\n",
                   address, SIZE * frame_idx + offset, result);

        }
    }

    printf("TLB hit rate: %f  page fault rate is %f\n",TLBhit/1000.0,pagefault/1000.0);
    return 0;
}
void initial()
{
    for (int i = 0; i < MAX_PAGE_NUMBER; i++)
        pagetable[i] = -1;

    memset(phyMem, 0, sizeof(phyMem));

    for (int i = 0; i < MAX_TLB_NUMBER;i++)
    {
        TLB[i].pa = -1;
        TLB[i].va = -1;
        TLB[i].lru = MAX_IN_SIZE;
    }
    for (int i = 0; i < MAX_FRAME_NUMBER;i++)
        memory_lru[i] = 0;
    return;
}

int testTLB(int page_idx)
{
    int index = page_idx;
    for (int i = 0; i < MAX_TLB_NUMBER;i++)
    {
        if(TLB[i].va==index)
            return i;
    }
    return -1;
}

int updateTLBLRU(int page_idx,int frame_idx)
{
    int index = -1;
    for (int i = 0; i < MAX_TLB_NUMBER;i++)
    {
        if(TLB[i].va==page_idx)
            {
                index = i;
                TLB[i].lru = MAX_IN_SIZE;
                break;
            }
    }

    if(index==-1)
    {
        int min_lru = TLB[0].lru;
        int min_lru_index = 0;
        for (int i = 1; i < MAX_TLB_NUMBER; i++)
        {
            if(TLB[i].lru<min_lru)
            {
                min_lru_index = i;
                min_lru = TLB[i].lru;
            }
        }
        TLB[min_lru_index].lru = MAX_IN_SIZE;
        TLB[min_lru_index].va = page_idx;
        TLB[min_lru_index].pa = frame_idx;
    }

    for (int i = 0; i < MAX_TLB_NUMBER;i++)
        TLB[i].lru = TLB[i].lru - 1;

    return 0;
}

int updateFrameLRU(int frame_idx)
{
    memory_lru[frame_idx] = MAX_IN_SIZE;
    for (int i = 0; i < MAX_FRAME_NUMBER;i++)
        memory_lru[i] = memory_lru[i] - 1;
}

int searchPageTable(int page_idx)
{
   
    if(pagetable[page_idx]!=-1)
        return 0;
    else
        return -1;
}

int findFreeFrame(int page_idx)
{
    
    for (int i = 0; i < MAX_FRAME_NUMBER;i++)
    if(memory_lru[i]<=0)
        return i;

    int min_lru = memory_lru[0];

    int min_lru_index = 0;
        for (int i = 0; i < MAX_FRAME_NUMBER; i++)
        {
            if (memory_lru[i] < min_lru)
            {
                min_lru_index = i;
                min_lru = memory_lru[i];
            }
        }
        for (int i = 0; i < MAX_PAGE_NUMBER;i++)
        {
            if(pagetable[i]==min_lru_index)
                pagetable[i] = -1;
            break;
        }
            return min_lru_index;
}