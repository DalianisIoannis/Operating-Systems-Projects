#include "functions.h"

int Sem_Init(key_t key, int nsems, int value){
    int sem_id, i, error;
    union semun arg;
    if( key<0 || nsems<=0 ){
        fprintf(stderr, "Failed to initialize semaphore\n");
        return -1;
    }
    // If IPC_EXCL is used along with IPC_CREAT, then either 
    // a new set is created, or if the set exists, 
    // the call fails with -1.
    sem_id = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0600);
    if(sem_id<0){
        fprintf(stderr, "Failed to initialize semaphore\n");
        return -1;
    }
    arg.val = value;
    for(i=0; i<nsems; i++){
        if( error = semctl(sem_id, i, SETVAL, arg)<0 ){
            fprintf(stderr, "Failed to initialize semaphore in nsems.\n");
            return -1;
        }
    }
    return sem_id;
}

int Sem_Down(int sem_id, int sem_num){ //P()
    struct sembuf sbuf;
    if( sem_id<0 || sem_num<0 ){
        fprintf(stderr, "Failed to sem Down\n");
        return -1;
    }
    sbuf.sem_num = sem_num;
    sbuf.sem_op = -1;
    // SEM_UNDO flag undo operations after process terminates
    sbuf.sem_flg = SEM_UNDO;
    return semop(sem_id, &sbuf, 1);
}

int Sem_Up(int sem_id, int sem_num){ //V()
    struct sembuf sbuf;
    if( sem_id<0 || sem_num<0 ){
        fprintf(stderr, "Failed to sem Up.\n");
        return -1;
    }
    sbuf.sem_num = sem_num;
    sbuf.sem_op = 1;
    // SEM_UNDO flag undo operations after process terminates
    sbuf.sem_flg = SEM_UNDO;
    return semop(sem_id, &sbuf, 1);
}

int Sem_Del(int sem_id){
    if(sem_id<0){
        fprintf(stderr, "Failed to del sem.\n");
        return -1;
    }
    return semctl(sem_id, 0, IPC_RMID);
}