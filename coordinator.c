#include "functions.h"

int main(int argc, char** argv){
    // argv[1] number of peers
    // argv[2] number of entries
    // argv[3] float percentage of readers
    // argv[4] float percentage of writers
    // argv[5] int number of repeats
    int     i, n, j, peers_num, entries_num, sem_key, ShM_id, status;
    int     isRdr_Wrtr, writes_made, reads_made, rep_num, parent_of_parent;
    double  time_taken;
    pid_t   pid;
    float   rdrs_num, wrtrs_num;
    key_t   key;
    Entry   ShMPtr;   // ShMData *ShMPtr;
    FILE    *temp_file;

    if (argc != 6) {
		fprintf(stderr, "Usage: need 6 arguments!\n");
        exit(0);
    }
    temp_file   = fopen("temp_file.txt", "w");
    peers_num   = atoi(argv[1]);
    entries_num = atoi(argv[2]);
    rdrs_num    = (float)(atof(argv[3]));
    wrtrs_num   = (float)(atof(argv[4]));
    rep_num     = atoi(argv[5]);
    if( (rdrs_num+wrtrs_num) != 1.0f ){
        fprintf(stderr, "Wrong percentage of readers-writers.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(temp_file, "Num of peers: %d, num of entries: %d, num of repititions: %d, percentage of readers: %f and percentage of writers: %f.\n", peers_num, entries_num, rep_num, rdrs_num, wrtrs_num);

    key = ftok("coordinator.c", 'R');
    if(key<0){
        fprintf(stderr, "Failed to make shared memory key.\n");
        exit(EXIT_FAILURE);
    }
    ShM_id = ShMInit(key, entries_num);
    if(ShM_id<0){
        exit(EXIT_FAILURE);
    }
    ShMPtr = ShMAttach(ShM_id);
    if( ShMPtr == NULL ){
        fprintf(stderr, "Failed to attach shm.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(temp_file, "Initial array of shared memory entries is:\n");
    sem_key = 5768;
    for(i=0; i<entries_num; i++){

        ShMPtr[i].rw_mutex  = Sem_Init( (key_t)sem_key, 1, 1 );
        if(ShMPtr[i].rw_mutex<0){
            exit(EXIT_FAILURE);
        }
        sem_key++;
        ShMPtr[i].mutex     = Sem_Init( (key_t)sem_key, 1, 1 );
        if(ShMPtr[i].mutex<0){
            exit(EXIT_FAILURE);
        }
        sem_key++;
        ShMPtr[i].value       = i+1;
        ShMPtr[i].reads_made  = 0;
        ShMPtr[i].writes_made = 0;
        ShMPtr[i].read_count  = 0;

        fprintf(temp_file,"Entry %d:\n\tvalue = %d\n", i+1, ShMPtr[i].value);
        fprintf(temp_file, "\tread and write semaphore key = %d\n", ShMPtr[i].rw_mutex);
        fprintf(temp_file, "\tmutual exclusion semaphore key = %d\n", ShMPtr[i].mutex);
        fprintf(temp_file, "\treads made = %d\n\twrites made = %d\n", ShMPtr[i].reads_made, ShMPtr[i].writes_made);

    }
    fprintf(temp_file, "\nExecution of peers:\n");
    fclose(temp_file);
    
    parent_of_parent = (int)(getppid());
    print_whoami(parent_of_parent);
    // child array
    pid_t pids[peers_num];
    for (i = 0; i < peers_num; ++i) {
        // sleep(15);
        pids[i] = fork();

        if ( pids[i] < 0 ) {
            fprintf(stderr, "Fork failed.\n");
			exit(EXIT_FAILURE);
        }
        else if (pids[i] == 0) {
            srand(getpid());
            reads_made  = 0;
            writes_made = 0;
            time_taken  = 0.0;

            print_whoami(parent_of_parent);
            for(j=0; j<rep_num; j++){

                isRdr_Wrtr = read_or_write( rdrs_num, wrtrs_num );
                if(isRdr_Wrtr==1){
                    reads_made++;
                }
                else{
                    writes_made++;
                }
                temp_file = fopen("temp_file.txt", "a");
                time_taken += proc_func(isRdr_Wrtr, ShMPtr, entries_num, temp_file);
                if(time_taken<0.0){
                    fprintf(stderr, "Error at peer function.\n");
                    exit(EXIT_FAILURE);
                }
                fclose(temp_file);

            }
            temp_file = fopen("temp_file.txt", "a");
            fprintf(temp_file, "For peer %d total time counted is %f, readings done are %d, writes done are %d and average time is %f.\n\n", getpid(), time_taken, reads_made, writes_made, time_taken/(float)entries_num);
            fclose(temp_file);
            exit(0);
        }
    }

    /* Wait for children to exit. */
    n=peers_num;
    while (n > 0) {
        pid = wait(&status);
        printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
        --n;  //  Remove pid from the pids array.
    }

    reads_made  = 0;
    writes_made = 0;
    temp_file = fopen("temp_file.txt", "a");
    fprintf(temp_file, "Final form of shared memory entries:\n");
    for(i=0; i<entries_num; i++){

        fprintf(temp_file,"Entry %d:\n\tvalue = %d\n", i+1, ShMPtr[i].value);
        fprintf(temp_file, "\tread and write semaphore key = %d\n", ShMPtr[i].rw_mutex);
        fprintf(temp_file, "\tmutual exclusion semaphore key = %d\n", ShMPtr[i].mutex);
        fprintf(temp_file, "\treads made = %d\n\twrites made = %d\n", ShMPtr[i].reads_made, ShMPtr[i].writes_made);
        reads_made += ShMPtr[i].reads_made;
        writes_made += ShMPtr[i].writes_made;

        if( Sem_Del(ShMPtr[i].rw_mutex)<0 ){
            exit(EXIT_FAILURE);
        }
        if( Sem_Del(ShMPtr[i].mutex) ){
            exit(EXIT_FAILURE);
        }
    }
    print_whoami(parent_of_parent);
    fprintf(temp_file, "\nTotal reads made by peers: %d and total writes: %d.\n", reads_made, writes_made);
    if( ShMDettach(ShMPtr)<0 ){
        fprintf(stderr, "Failed to dettach shm.\n");
        exit(EXIT_FAILURE);
    }
    if( ShMDestroy(ShM_id)<0 ){
        fprintf(stderr, "Failed to destroy shm.\n");
        exit(EXIT_FAILURE);
    }
    printf("Open text file to check.\n");
    fclose(temp_file);

    return 0;
}