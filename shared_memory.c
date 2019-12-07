#include "sems_shm.h"

int ShMInit(key_t key, int entries_num){
    if(key<0){
        fprintf(stderr, "Failed to init ShM.\n");
        return -1;
    }
    return shmget(key, entries_num*sizeof(Entry), IPC_CREAT | 0666);
}

Entry ShMAttach(int ShM_id){
    return shmat(ShM_id, (void*) 0, 0);
}

// int ShMDettach(Entry ShM_pointer){
int ShMDettach(ShMData* ShM_pointer){
    return shmdt(ShM_pointer);
}

int ShMDestroy(int ShM_id){
    return shmctl(ShM_id, IPC_RMID, 0);
}