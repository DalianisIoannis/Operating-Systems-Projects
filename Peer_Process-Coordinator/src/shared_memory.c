#include "../headers/functions.h"

int ShMInit(key_t key, int entries_num){
    if(key<0){ return -1; }
    int shmid = shmget(key, entries_num*sizeof(ShMData), IPC_CREAT | 0666);
    return shmid;
}

Entry ShMAttach(int ShM_id){
    return shmat(ShM_id, (void*) 0, 0);
}

int ShMDettach(Entry ShM_pointer){
    return shmdt(ShM_pointer);
}

int ShMDestroy(int ShM_id){
    return shmctl(ShM_id, IPC_RMID, 0);
}