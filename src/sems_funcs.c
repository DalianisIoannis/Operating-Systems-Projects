#include "../headers/functions.h"

int Sem_Init(key_t key, int nsems, int value){
    int sem_id, i;
    union semun arg;

    if( key<0 || nsems<=0 ){ return -1; }
    
    // If IPC_EXCL is used along with IPC_CREAT, then either 
    // a new set is created, or if the set exists, 
    // the call fails with -1.
    // sem_id = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0600);
    sem_id = semget(key, nsems, 0600 | IPC_CREAT);
    if(sem_id<0){ return -1; }
    
    arg.val = value;
    for(i=0; i<nsems; i++){
        if( semctl(sem_id, i, SETVAL, arg)<0 ){ return -1; }
    }
    
    return sem_id;
}

int Sem_Down(int sem_id, int sem_num){ //P()
    struct sembuf sbuf;

    if( sem_id<0 || sem_num<0 ){ return -1; }

    sbuf.sem_num = sem_num;
    sbuf.sem_op = -1;
    // SEM_UNDO flag undo operations after process terminates
    sbuf.sem_flg = SEM_UNDO;
    
    return semop(sem_id, &sbuf, 1);
}

int Sem_Up(int sem_id, int sem_num){ //V()
    struct sembuf sbuf;

    if( sem_id<0 || sem_num<0 ){ return -1; }

    sbuf.sem_num = sem_num;
    sbuf.sem_op = 1;
    // SEM_UNDO flag undo operations after process terminates
    sbuf.sem_flg = SEM_UNDO;

    return semop(sem_id, &sbuf, 1);
}

int Sem_Del(int sem_id){
    union semun arg;
    if(sem_id<0){ return -1; }
    return semctl(sem_id, 0, IPC_RMID, arg);
}