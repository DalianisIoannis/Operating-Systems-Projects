#include "../headers/functions.h"
#include "../headers/ShMFuncs.h"
#include "../headers/SemFuncs.h"

void print_whoami(int id){
    if( getppid()!=id ){
        printf("Child created with pid: %d and ppid: %d.\n", getpid(), getppid() );
    }
    else{
        printf("This is parent process with id: %d and ppid: %d.\n", getpid(), getppid());
    }
}

void fnExit (int id, Entry ptr){
    ShMDettach(ptr);
    ShMDestroy(id);
}

Entry initSharedMem(int ShM_id, int arSize) {
    
    Entry   ShMPtr;
    long    sem_key;

    if( (ShMPtr = ShMAttach(ShM_id)) == NULL ){
        ShMDestroy(ShM_id);
        fprintf(stderr, "Failed to attach shm.\n");
        return NULL;
    }

    sem_key = 5768;

    if( (ShMPtr->mutex  = Sem_Init( (key_t)sem_key, 1, 1 ))<0 ){
        fnExit(ShM_id, ShMPtr);
        fprintf(stderr, "Failed to initialize semaphore mutex.\n");
        return NULL;
    }

    sem_key++;

    if( (ShMPtr->semA  = Sem_Init( (key_t)sem_key, 1, 1 ))<0 ){
        fnExit(ShM_id, ShMPtr);
        fprintf(stderr, "Failed to initialize semaphore semA.\n");
        return NULL;
    }

    sem_key++;

    ShMPtr->count = 0;

    for(int i=0; i<2; i++) {
        if( ((ShMPtr->semArray)[i] = Sem_Init( (key_t)sem_key, 1, 0 ))<0 ) {
            fprintf(stderr, "Failed to initialize semaphore for entry %d.\n", i);
            fnExit(ShM_id, ShMPtr);
            return NULL;
        }
        sem_key++;
    }

    return ShMPtr;

}

int feeder(int numProc, int arSize) {

    key_t   key;
    int     status, ShM_id, i;
    int     *intAr;
    pid_t   pid;
    Entry   ShMPtr;   // ShMData *ShMPtr;
    FILE    *temp_file;
    struct timespec ender;
    double time_taken;

    intAr = malloc(arSize*sizeof(int));

    if(intAr==NULL) {
        fprintf(stderr, "Failed to malloc array.\n");
        return -1;
    }

    srand(time(NULL));

    for(i=0; i<arSize; i++) {
        intAr[i] = rand();
    }

    // printf("Array in feeder is:\n");
    // for(i=0; i<arSize; i++) {
    //     printf("%d\n", intAr[i]);
    // }

    key = ftok("./src/feeder.c", 'R');
    if( (ShM_id = ShMInit(key, 1))<0 ){
        fprintf(stderr, "Failed to init ShM.\n");
        return -1;
    }

    ShMPtr = initSharedMem(ShM_id, arSize);

    temp_file = fopen("./temp_file.txt", "w");

    int parent_of_parent = (int)(getppid());
    print_whoami( parent_of_parent);

    pid_t pids[numProc];  // child array
    int index = 0;  // for changing between semaphores

    for (i = 0; i < numProc; ++i) {
        pids[i] = fork();

        if ( pids[i] < 0 ) {
            fnExit(ShM_id, ShMPtr);
            fprintf(stderr, "Fork failed.\n");
            return -1;
        }
        else if (pids[i] == 0) {

            print_whoami(parent_of_parent);

            int *ProcAr;

            ProcAr = malloc(arSize*sizeof(int));
            if(ProcAr==NULL) {
                fprintf(stderr, "Failed to malloc array.\n");
                return -1;
            }

            double total_time_taken=0.0;

            for (int k = 0; k < arSize; ++k) {

                if( (Sem_Down( (ShMPtr->semArray)[index], 0 ))<0 ){
                    fprintf(stderr, "Failed to sem Down at line %d.\n", __LINE__);
                    return -1;
                }

                ProcAr[k] = ShMPtr->buf;
                index = 1 - index;

                clock_gettime(CLOCK_MONOTONIC, &ender);
                time_taken = (ender.tv_sec - (ShMPtr->starter).tv_sec) * 1e9;
                time_taken = (time_taken + (ender.tv_nsec - (ShMPtr->starter).tv_nsec)) * 1e-9;

                total_time_taken = total_time_taken + time_taken;
                
                if( (Sem_Down( ShMPtr->mutex, 0 ))<0 ){
                    fprintf(stderr, "Failed to sem Down at line %d.\n", __LINE__);
                    return -1;
                }

                (ShMPtr->count)++;

                if( ShMPtr->count==numProc ) {
                                        
                    (ShMPtr->count)=0;

                    if( (Sem_Up( ShMPtr->semA, 0 ))<0 ){
                        fprintf(stderr, "Failed to sem Up at line %d.\n", __LINE__);
                        return -1;
                    }

                }
                
                if( (Sem_Up( ShMPtr->mutex, 0 ))<0 ){
                    fprintf(stderr, "Failed to sem Up at line %d.\n", __LINE__);
                    return -1;
                }

            }

            if( (Sem_Down( ShMPtr->mutex, 0 ))<0 ){
                fprintf(stderr, "Failed to sem Down at line %d.\n", __LINE__);
                return -1;
            }

            if (i == numProc-1) { // last 
                fprintf(temp_file, "Array in process %d(last that finished) is:\n", (int)getpid());
                for(i=0; i<arSize; i++) {
                    fprintf(temp_file, "%d\n", ProcAr[i]);
                }
                
                printf("Total delay time: %f nanoseconds\n", total_time_taken);
                printf("Average total delay time: %f nanoseconds\n", (double)(total_time_taken/(double)arSize));
                fprintf(temp_file, "Average total delay time: %f nanoseconds\n", (double)(total_time_taken/(double)arSize));
            }            

            if( (Sem_Up( ShMPtr->mutex, 0 ))<0 ){
                fprintf(stderr, "Failed to sem Up at line %d.\n", __LINE__);
                return -1;
            }

            free(intAr);
            free(ProcAr);
            fclose(temp_file);

            exit(0);
        }
    }

    for (int k = 0; k < arSize; ++k) {
        
        if( (Sem_Down( ShMPtr->semA, 0 ))<0 ){
            fprintf(stderr, "Failed to sem Down at line %d.\n", __LINE__);
            return -1;
        }

        ShMPtr->buf = intAr[k];
        clock_gettime(CLOCK_MONOTONIC, &(ShMPtr->starter));
        
        for(int kk=0; kk<numProc; kk++) {

            if( (Sem_Up( (ShMPtr->semArray)[index], 0 ))<0 ){
                return -1;
            }
        }

        index = 1 - index;
    }

    /* Wait for children to exit. */
    int n=numProc;
    while (n > 0) {
        pid = wait(&status);
        printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
        --n;  //  Remove pid from the pids array.
    }

    fclose(temp_file);

    if( Sem_Del(ShMPtr->mutex)<0 ){
        fnExit(ShM_id, ShMPtr);
        fprintf(stderr, "Failed to del mutex.\n");
        return -1;
    }
    if( Sem_Del(ShMPtr->semA)<0 ){
        fnExit(ShM_id, ShMPtr);
        fprintf(stderr, "Failed to del semA.\n");
        return -1;
    }

    for(i=0; i<2; i++) {
        if( Sem_Del((ShMPtr->semArray)[i])<0 ){
            fprintf(stderr, "Failed to delete semaphore for entry %d.\n", i);
            fnExit(ShM_id, ShMPtr);
            return -1;
        }
    }

    if( ShMDettach(ShMPtr)<0 ){
        fprintf(stderr, "Failed to dettach shm.\n");
        return -1;
    }
    if( ShMDestroy(ShM_id)<0 ){
        fprintf(stderr, "Failed to destroy shm.\n");
        return -1;
    }

    free(intAr);

    return 0;
}