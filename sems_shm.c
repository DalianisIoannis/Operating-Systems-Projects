#include "sems_shm.h"
// // // // // // // // // // // // // // // //
// // // // // // // // // // // // // // // //semaphores
int Sem_Init(key_t key, int nsems, int value){
    int sem_id, i, error;
    union semun arg;

    if( key<0 || nsems<=0 ){
        fprintf(stderr, "Failed to initialize semaphore\n");
        // return -1;
        exit(EXIT_FAILURE);
    }
    // If IPC_EXCL is used along with IPC_CREAT, then either 
    // a new set is created, or if the set exists, 
    // the call fails with -1.
    sem_id = semget(key, nsems, 0666 | IPC_CREAT);
    // sem_id = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0600);

    if(sem_id<0){
        fprintf(stderr, "Failed to initialize semaphore\n");
        // return -1;
        exit(EXIT_FAILURE);
    }

    arg.val = value;

    for(i=0; i<nsems; i++){
        if( error = semctl(sem_id, i, SETVAL, arg)<0 ){
            // return -1;
            // exit(0);
            exit(EXIT_FAILURE);
        }
    }

    return sem_id;
}

int Sem_Down(int sem_id, int sem_num){ //V()
    struct sembuf sbuf;

    if( sem_id<0 || sem_num<0 ){
        fprintf(stderr, "Failed to sem Down\n");
        // return -1;
        exit(EXIT_FAILURE);
    }
    
    sbuf.sem_num = sem_num;
    sbuf.sem_op = 1;
    // sbuf.sem_flg = 0;
    // SEM_UNDO flag undo operations after process terminates
    sbuf.sem_flg = SEM_UNDO;

    return semop(sem_id, &sbuf, 1);
    // return semop(sem_id, &sbuf, sizeof(sbuf));
    
}

int Sem_Up(int sem_id, int sem_num){ //P()
    struct sembuf sbuf;

    if( sem_id<0 || sem_num<0 ){
        fprintf(stderr, "Failed to sem Up.\n");
        // return -1;
        exit(EXIT_FAILURE);
    }
    
    sbuf.sem_num = sem_num;
    sbuf.sem_op = -1;
    // sbuf.sem_flg = 0;
    // SEM_UNDO flag undo operations after process terminates
    sbuf.sem_flg = SEM_UNDO;

    return semop(sem_id, &sbuf, 1);
    // return semop(sem_id, &sbuf, sizeof(sbuf));
    
}

int Sem_Get(int sem_id, int n){
    union semun arg;
    if(sem_id<0 || n<0){
        fprintf(stderr, "Failed to get sem.\n");
        // return -1;
        exit(EXIT_FAILURE);
    }
    return semctl(sem_id, n, GETVAL, arg);
}

int Sem_Set(int sem_id, int n, int val){
    union semun arg;
    if(sem_id<0 || n<0){
        fprintf(stderr, "Failed to set sem.\n");
        // return -1;
        exit(EXIT_FAILURE);
    }

    arg.val = val;

    return semctl(sem_id, n, SETVAL, arg);
}

int Sem_Del(int sem_id){
    union semun sem_union;
    if(sem_id<0){
        fprintf(stderr, "Failed to del sem.\n");
        // return -1;
        exit(EXIT_FAILURE);
    }

    // return semctl(sem_id, 0, IPC_RMID, sem_union);
    return semctl(sem_id, 0, IPC_RMID);
}
// // // // // // // // // // // // // // // //
// // // // // // // // // // // // // // // //shared memory
int ShMInit(key_t key){
    if(key<0){
        fprintf(stderr, "Failed to init ShM.\n");
        return -1;
    }
    return shmget(key, sizeof(ShMData), IPC_CREAT | 0666);
}

ShMData* ShMAttach(int ShM_id){
    return shmat(ShM_id, (void*) 0, 0);
}

int ShMDettach(ShMData* ShM_pointer){
    return shmdt(ShM_pointer);
}

int ShMDestroy(int ShM_id){
    return shmctl(ShM_id, IPC_RMID, 0);
}