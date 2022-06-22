#pragma once

#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct ShMData{   // entry
    int buf;  // current buffer
    int count;
    int semA;
    int mutex;
    int semArray[2];
    struct timespec starter;
}ShMData;

typedef ShMData* Entry;

int ShMInit(key_t key, int entries_num);    // init shared memory

ShMData *ShMAttach(int ShM_id);             // get pointer to shared memory

int ShMDettach(ShMData* ShM_pointer);       // dettach shared memory

int ShMDestroy(int ShM_id);