#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}arg;

// struct sembuf{
//     unsigned short sem_num;
//     unsigned short sem_op; // sem operation
//     unsigned short sem_flg;
// };

// entry
typedef struct data{
    int value;
    int rdr_count;
    int wrt_count;
    double time_consumed;
    int semr;
}ShMData;

typedef ShMData* Entry;

// semaphores initialize
int Sem_Init(key_t key, int nsems, int value);
// P() wait sem
int Sem_Down(int sem_id, int sem_num);
// V() signal sem
int Sem_Up(int sem_id, int sem_num);
// semaphore destroy
int Sem_Del(int sem_id);
// Get sem value
int Sem_Get(int sem_id, int n);
// set sem value
int Sem_Set(int sem_id, int n, int val);

// init shared memory
int ShMInit(key_t key, int entries_num);
// int ShMInit(key_t key);
// get pointer to shared memory
ShMData *ShMAttach(int ShM_id);
// dettach shared memory
int ShMDettach(ShMData* ShM_pointer);
// destroy shared memory
int ShMDestroy(int ShM_id);

// process is reader or writer?
int read_or_write(float read_per, float wrt_per);
int read_write(int* rdrs, int* wrts);
// function to be executed by processes
void proc_func(int isrd_wrt, Entry mentry, int entrs, FILE* temp_file);