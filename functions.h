#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define MULTIPLIER 5
#define LEXP 2

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
    int read_count;
    int writes_made;
    int reads_made;
    /*common for reading and writing sem for mutual exclusion of writers*/
    int rw_mutex;
    /*for mutual exclusion when informing read_count*/
    int mutex;
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

// init shared memory
int ShMInit(key_t key, int entries_num);
// get pointer to shared memory
ShMData *ShMAttach(int ShM_id);
// dettach shared memory
int ShMDettach(ShMData* ShM_pointer);
// destroy shared memory
int ShMDestroy(int ShM_id);

// process is reader or writer?
int read_or_write(float read_per, float wrt_per);
// function to be executed by processes
double proc_func(int isrd_wrt, Entry mentry, int entrs, FILE* temp_file);

void print_whoami(int id);