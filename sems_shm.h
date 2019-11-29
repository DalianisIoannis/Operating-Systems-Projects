#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/shm.h>

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}arg;

// struct sembuf
// {
//     unsigned short sem_num;
//     unsigned short sem_op; // sem operation
//     unsigned short sem_flg;
// };

// struct to save in shared memory
typedef struct data
{
    int value;
    int count;
    double time_consumed;
}ShMData;


// semaphores init
int Sem_Init(key_t, int, int);
// P() wait sem
int Sem_Down(int, int);
// V() signal sem
int Sem_Up(int, int);
// semaphore destroy
int Sem_Del(int);
// Get sem value
int Sem_Get(int, int);
// set sem value
int Sem_Set(int, int, int);

// init shared memory
int ShMInit(key_t);
// get pointer to shared memory
ShMData *ShMAttach(int);
// dettach shared memory
int ShMDettach(ShMData*);
// destroy shared memory
int ShMDestroy(int);