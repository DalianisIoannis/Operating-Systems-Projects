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
    sem_id = semget(key, nsems, 0666 | IPC_CREAT);
    // sem_id = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0600);
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

int Sem_Get(int sem_id, int n){
    union semun arg;
    if(sem_id<0 || n<0){
        fprintf(stderr, "Failed to get sem.\n");
        return -1;
        // exit(EXIT_FAILURE);
    }
    return semctl(sem_id, n, GETVAL, arg);
}

int Sem_Set(int sem_id, int n, int val){
    union semun arg;
    if(sem_id<0 || n<0){
        fprintf(stderr, "Failed to set sem.\n");
        return -1;
        // exit(EXIT_FAILURE);
    }

    arg.val = val;

    return semctl(sem_id, n, SETVAL, arg);
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
    float a = 1.0;
    float x = (float)rand()/(float)(RAND_MAX/a);
    // printf("%f\n", x);
    if(x<=read_per){
        return 1;
    }
    else{
        return 0;
    }
}

void proc_func(int isrd_wrt, Entry mentry, int entrs, FILE* temp_file){
    // https://www.tutorialspoint.com/how-to-measure-time-taken-by-a-function-in-c
    // https://www.geeksforgeeks.org/how-to-measure-time-taken-by-a-program-in-c/
    int rand_entr = rand()%entrs;
    if(isrd_wrt==1){//is reader
        // clock_t start, end;
        // double time_taken;
        // start = clock();

        int sm_rd = Sem_Down( mentry[rand_entr].semr, 0 );
        // clock_t start, end;
        // double time_taken;
        // start = clock();

        // poio apo ta dio rdr_count++;
        mentry[rand_entr].rdr_count++;
        fprintf(temp_file, "Read entry %d with value= %d with process pid %d\n",rand_entr, mentry[rand_entr].value, getpid());
        
        // end = clock();// - t;
        // time_taken = ( (double)(end-start) ) / CLOCKS_PER_SEC; // calculate the elapsed time
        // printf("Metrisa xrono %f\n", time_taken);
        // mentry[rand_entr].time_consumed += time_taken;
        sm_rd = Sem_Up( mentry[rand_entr].semr, 0 );

        // end = clock();// - t;
        // time_taken = ( (double)(end-start) ) / CLOCKS_PER_SEC; // calculate the elapsed time
        
        // int sm_wrt = Sem_Down( mentry[rand_entr].semr, 0 );
        // mentry[rand_entr].time_consumed += time_taken;
        // mentry[rand_entr].rdr_count++;
        // sm_wrt = Sem_Up( mentry[rand_entr].semr, 0 );

    }
    else{//is writer
        fprintf(temp_file, "Write at entry %d with process pid %d\n",rand_entr, getpid());
        // printf("Write at entry %d with process pid %d\n",rand_entr, getpid());
        int sm_wrt = Sem_Down( mentry[rand_entr].semr, 0 );
        // clock_t start, end;
        // double time_taken;
        // start = clock();

        // sleep(10);
        mentry[rand_entr].value++;
        mentry[rand_entr].wrt_count++;

        // end = clock();// - t;
        // time_taken = ( (double)(end-start) ) / CLOCKS_PER_SEC; // calculate the elapsed time
        // printf("Metrisa xrono %f\n", time_taken);
        // mentry[rand_entr].time_consumed += time_taken;

        sm_wrt = Sem_Up( mentry[rand_entr].semr, 0 );
    }
}