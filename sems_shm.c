#include "sems_shm.h"
// // // // // // // // // // // // // // // //
// // // // // // // // // // // // // // // //semaphores

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
    // sem_id = semget(key, nsems, 0666 | IPC_CREAT);
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
            // exit(0);
        }
    }
    return sem_id;
}

int Sem_Down(int sem_id, int sem_num){ //P()
    struct sembuf sbuf;

    if( sem_id<0 || sem_num<0 ){
        fprintf(stderr, "Failed to sem Down\n");
        return -1;
        // exit(EXIT_FAILURE);
    }
    
    sbuf.sem_num = sem_num;
    sbuf.sem_op = -1;
    // sbuf.sem_flg = 0;
    // SEM_UNDO flag undo operations after process terminates
    sbuf.sem_flg = SEM_UNDO;

    return semop(sem_id, &sbuf, 1);
    // return semop(sem_id, &sbuf, sizeof(sbuf));
}

int Sem_Up(int sem_id, int sem_num){ //V()
    struct sembuf sbuf;

    if( sem_id<0 || sem_num<0 ){
        fprintf(stderr, "Failed to sem Up.\n");
        return -1;
        // exit(EXIT_FAILURE);
    }
    
    sbuf.sem_num = sem_num;
    sbuf.sem_op = 1;
    // sbuf.sem_flg = 0;
    // SEM_UNDO flag undo operations after process terminates
    sbuf.sem_flg = SEM_UNDO;

    return semop(sem_id, &sbuf, 1);
    // return semop(sem_id, &sbuf, sizeof(sbuf));
}

int Sem_Del(int sem_id){
    union semun sem_union;
    if(sem_id<0){
        fprintf(stderr, "Failed to del sem.\n");
        return -1;
        // exit(EXIT_FAILURE);
    }
    // return semctl(sem_id, 0, IPC_RMID, sem_union);
    return semctl(sem_id, 0, IPC_RMID);
}

int read_write(int* rdrs, int* wrts){
    if( rand()%2==1 ){
        if( (*rdrs)!=0 ){
            (*rdrs)--;
            return 1;
        }
        (*wrts)--;
        return 0;
    }
    if( (*wrts)!=0 ){
        (*wrts)--;
        return 0;
    }
    (*rdrs)--;
    return 1;
}

int read_or_write(float read_per, float wrt_per){
    // srand(getpid());
    // srand(time(NULL));
    float a = 1.0;
    float x = (float)rand() / (float)(RAND_MAX/a);
    // printf("%f\n", x);
    if(x<=read_per){
        return 1;
    }
    else{
        return 0;
    }
}

void proc_func(int isrd_wrt, Entry mentry, int entrs, FILE* temp_file){
    // srand(getpid());
    // srand(time(NULL));
    double T = MULTIPLIER*( -log((double)rand()/(double) RAND_MAX) / LEXP  );
    // printf("Wait for %f\n", T);
    // sleep(T);
    int rand_entr = rand()%entrs;
    int sem_first_read;
    int random_int;

    if(isrd_wrt==1){//is reader
        int sm_rd = Sem_Down( mentry[rand_entr].mutex, 0 );

        mentry[rand_entr].rdr_count++;  //read_count
        if( mentry[rand_entr].rdr_count==1 ){
            sem_first_read = Sem_Down( mentry[rand_entr].rw_mutex, 0 );
        }
        sm_rd = Sem_Up( mentry[rand_entr].mutex, 0 );
        /* reading */
        fprintf(temp_file, "Read entry %d with value = %d with process pid %d\n",rand_entr+1, mentry[rand_entr].value, getpid());

        sm_rd = Sem_Down( mentry[rand_entr].mutex, 0 );
        mentry[rand_entr].rdr_count--;
        if( mentry[rand_entr].rdr_count==0 ){
            sem_first_read = Sem_Up( mentry[rand_entr].rw_mutex, 0 );
        }
        sm_rd = Sem_Up( mentry[rand_entr].mutex, 0 );
    }
    else{//is writer
        
        int sm_wrt = Sem_Down( mentry[rand_entr].rw_mutex, 0 );
        random_int = rand()%100+1;
        fprintf(temp_file, "Write %d at entry %d with process pid %d\n",random_int  ,rand_entr+1, getpid());
        mentry[rand_entr].value = random_int;
        mentry[rand_entr].wrt_count++;

        sm_wrt = Sem_Up( mentry[rand_entr].rw_mutex, 0 );
    }
}