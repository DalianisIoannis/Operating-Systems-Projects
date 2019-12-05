// getpid(): obtain my own ID
// getppid(): get the ID of my parent
// #include <sys/timeb.h>
#include "sems_shm.h"

void print_whoami(const char *whoami){
        printf("%s created with pid: %d and ppid: %d\n", whoami, getpid(), getppid() );
}

int main(int argc, char** argv){
    // argv[1] number of peers
    // argv[2] number of entries
    // argv[3] float percentage of readers
    // argv[4] float percentage of writers
    // argv[5] int number of repeats
    int     sem_id, ShmID, i, n, j, peers_num, entries_num, sem_key, ShM_id;
    struct  shmid_ds Myshmid_ds;
    int     status = 0;
    pid_t   pid;
    float   rdrs_num, wrtrs_num, rep_num;
    int     isRdr_Wrtr; // 1 for reader 0 for writer
    key_t   key;
    Entry   ShMPtr;   // ShMData *ShMPtr;
    FILE    *temp_file;

    srand(time(NULL));
    // srand(getpid());

    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //check line input
    if (argc != 6) {
		fprintf(stderr, "Usage: need more arguments!\n");
        exit(0);
    }
    temp_file   = fopen("temp_file.txt", "w");
    peers_num   = atoi(argv[1]);
    entries_num = atoi(argv[2]);
    rdrs_num    = atof(argv[3]);
    wrtrs_num   = atof(argv[4]);
    rep_num     = atoi(argv[5]);
    int ar_entry[entries_num];
    fprintf(temp_file, "Num of peers: %d and num of entries: %d.\n", peers_num, entries_num);
    for(i=0; i<entries_num; i++){ ar_entry[i]=i+1; }
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // key = ftok("coordinator.c", getppid());
    key = ftok("coordinator.c", 'R');
    if(key<0){
        fprintf(stderr, "Failed to make shared memory key.\n");
        exit(EXIT_FAILURE);
    }
    ShM_id = ShMInit(key, entries_num);
    if(ShM_id<0){
        exit(EXIT_FAILURE);
    }
    // ShMPtr = (Entry) ShMAttach(ShM_id);
    ShMPtr = ShMAttach(ShM_id);
    // if( ShMPtr == (ShMData*)(-1) ){
    if( ShMPtr == NULL ){
        fprintf(stderr, "Failed to attach shm.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(temp_file, "Initial array of entry structs is:\n");
    sem_key = 5768;
    for(i=0; i<entries_num; i++){

        ShMPtr[i].rw_mutex = Sem_Init( (key_t)sem_key, 1, 1 );
        sem_key++;
        ShMPtr[i].mutex = Sem_Init( (key_t)sem_key, 1, 1 );
        sem_key++;
        ShMPtr[i].value = ar_entry[i];
        ShMPtr[i].rdr_count = 0;
        ShMPtr[i].wrt_count = 0;

        fprintf(temp_file,"Entry %d:\n\tvalue = %d\n", i+1, ShMPtr[i].value);
        fprintf(temp_file, "\tread and write semaphore key = %d\n", ShMPtr[i].rw_mutex);
        fprintf(temp_file, "\tmutual exclusion semaphore key = %d\n", ShMPtr[i].mutex);
        fprintf(temp_file, "\treads made = %d\n\twrites made = %d\n", ShMPtr[i].rdr_count, ShMPtr[i].wrt_count);

    }
    fprintf(temp_file, "\nExecution of peers:\n");
    fclose(temp_file);
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //fot_childs
    pid_t pids[peers_num];
    clock_t start, end;
    double time_taken;

    /* Start children. */
    for (i = 0; i < peers_num; ++i) {
        // isRdr_Wrtr = read_write(&rdrs_num, &wrtrs_num);
        // isRdr_Wrtr = read_or_write( rdrs_num, wrtrs_num );
        sleep(5);
        pids[i] = fork();
        // sleep(5);
        if ( pids[i] < 0 ) {
            fprintf(stderr, "Fork failed.\n");
			exit(EXIT_FAILURE);
        }
        else if (pids[i] == 0) {

            // sleep(5);

            print_whoami("Child");
            // sleep(5);
            start = clock();
            for(j=0; j<rep_num; j++){

                // sleep(5);
                // start = clock();

                // temp_file = fopen("temp_file.txt", "a");
                // fprintf(temp_file, "\nFor repetition %d time counted is \n", j);
                // fclose(temp_file);

                // sleep(5);
                isRdr_Wrtr = read_or_write( rdrs_num, wrtrs_num );
                temp_file = fopen("temp_file.txt", "a");
                proc_func(isRdr_Wrtr, ShMPtr, entries_num, temp_file);
                fclose(temp_file);

                // sleep(5);

                // temp_file = fopen("temp_file.txt", "a");
                // end = clock();// - t;
                // time_taken = (double)( ( (double)(end-start) ) / CLOCKS_PER_SEC ); // calculate the elapsed time
                // fprintf(temp_file, "%f.\n", time_taken);
                // fprintf(temp_file, "\nFor repetition %d time counted is %f.\n", j, time_taken);
                // fclose(temp_file);
                // sleep(5);

            }
            // sleep(5);
            temp_file = fopen("temp_file.txt", "a");
            end = clock();
            time_taken = (double)( ( (double)(end-start) ) / CLOCKS_PER_SEC ); // calculate the elapsed time
            fprintf(temp_file, "For peer %d time counted is %f.\n\n", getpid(), time_taken);
            fclose(temp_file);
            // sleep(5);

            exit(0);
        }
    }
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    /* Wait for children to exit. */
    n=peers_num;
    while (n > 0) {
        // sleep(5);
        pid = wait(&status);
        printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
        --n;  //  Remove pid from the pids array.
    }
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    temp_file = fopen("temp_file.txt", "a");
    fprintf(temp_file, "\nFinal form of shared memory entries:\n");
    for(i=0; i<entries_num; i++){

        fprintf(temp_file,"Entry %d:\n\tvalue = %d\n", i+1, ShMPtr[i].value);
        fprintf(temp_file, "\tread and write semaphore key = %d\n", ShMPtr[i].rw_mutex);
        fprintf(temp_file, "\tmutual exclusion semaphore key = %d\n", ShMPtr[i].mutex);
        fprintf(temp_file, "\treads made = %d\n\twrites made = %d\n", ShMPtr[i].rdr_count, ShMPtr[i].wrt_count);
        // fprintf(temp_file, "\ttime consumed in process = %f\n", ShMPtr[i].time_consumed);

        Sem_Del(ShMPtr[i].rw_mutex);
        Sem_Del(ShMPtr[i].mutex);
    }
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
