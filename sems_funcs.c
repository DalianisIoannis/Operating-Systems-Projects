#include "sems_shm.h"

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

int read_or_write(float read_per, float wrt_per){
    float a = 1.0;
    float x = (float)rand() / (float)(RAND_MAX/a);
    if(x<=read_per){
        return 1;   //reader
    }
    else{
        return 0;   //writer
    }
}

double proc_func(int isrd_wrt, Entry mentry, int entrs, FILE* temp_file){

    // exponential time
    double T = MULTIPLIER*( -log((double)rand()/(double) RAND_MAX) / LEXP  );
    clock_t start, end;
    double time_taken;
    int rand_entr = rand()%entrs;
    int sem_first_read;
    int random_int;
    int sm_rd;

    if(isrd_wrt==1){//is reader

        sm_rd = Sem_Down( mentry[rand_entr].mutex, 0 );
        mentry[rand_entr].read_count++;
        if( mentry[rand_entr].read_count==1 ){
            sem_first_read = Sem_Down( mentry[rand_entr].rw_mutex, 0 );
        }
        sm_rd = Sem_Up( mentry[rand_entr].mutex, 0 );

        /* reading */
        sleep(T);
        fprintf(temp_file, "Read entry %d with value = %d with process pid %d\n",rand_entr+1, mentry[rand_entr].value, getpid());
        mentry[rand_entr].reads_made++;

        start = clock();
        sm_rd = Sem_Down( mentry[rand_entr].mutex, 0 );
        end = clock();
        time_taken = (double)( ( (double)(end-start) ) / CLOCKS_PER_SEC );

        mentry[rand_entr].read_count--;
        if( mentry[rand_entr].read_count==0 ){
            sem_first_read = Sem_Up( mentry[rand_entr].rw_mutex, 0 );
        }
        sm_rd = Sem_Up( mentry[rand_entr].mutex, 0 );
    }
    else{//is writer
        
        start = clock();
        int sm_wrt = Sem_Down( mentry[rand_entr].rw_mutex, 0 );
        end = clock();
        time_taken = (double)( ( (double)(end-start) ) / CLOCKS_PER_SEC );

        random_int = rand()%100+1;
        sleep(T);
        fprintf(temp_file, "Write %d at entry %d with process pid %d\n",random_int  ,rand_entr+1, getpid());
        mentry[rand_entr].value = random_int;
        mentry[rand_entr].writes_made++;
        
        sm_wrt = Sem_Up( mentry[rand_entr].rw_mutex, 0 );
    }
    return time_taken;
}

void print_whoami(int id){
        if( getppid()!=id ){
            printf("Child created with pid: %d and ppid: %d\n", getpid(), getppid() );
        }
        else{
            printf("This is parent process id: %d and ppid: %d.\n", getpid(), getppid());
        }
}