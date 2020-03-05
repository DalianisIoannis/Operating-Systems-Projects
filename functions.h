#include <math.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/ipc.h>

#define MULTIPLIER 5
#define LEXP 2

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
}arg;

// struct sembuf{
//     unsigned short sem_num;
//     unsigned short sem_op; // sem operation
//     unsigned short sem_flg;
// };

typedef struct ShMData{   // entry
    int value;
    int read_count;
    int writes_made;
    int reads_made;
    int rw_mutex;   /*common for reading and writing sem for mutual exclusion of writers*/
    int mutex;      /*for mutual exclusion when informing read_count*/
}ShMData;

typedef ShMData* Entry;

int Sem_Init(key_t key, int nsems, int value);

int Sem_Down(int sem_id, int sem_num);  // P() wait sem

int Sem_Up(int sem_id, int sem_num);    // V() signal sem

int Sem_Del(int sem_id);


int ShMInit(key_t key, int entries_num);    // init shared memory

ShMData *ShMAttach(int ShM_id);             // get pointer to shared memory

int ShMDettach(ShMData* ShM_pointer);       // dettach shared memory

int ShMDestroy(int ShM_id);


int read_or_write(float read_per, float wrt_per);                           // process is reader or writer?

double proc_func(int isrd_wrt, Entry mentry, int entrs, FILE* temp_file);   // function to be executed by processes

void print_whoami(int id);