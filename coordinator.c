// getpid(): obtain my own ID
// getppid(): get the ID of my parent
#include <sys/timeb.h>
#include "sems_shm.h"

void print_whoami(const char *whoami){
        printf("%s created with pid: %d and ppid: %d\n", whoami, getpid(), getppid() );
}

int main(int argc, char** argv){
    // n = atoi(argv[1]);   number of peers
    // m = atoi(argv[2]);   number of entries
    // argv[3] number of readers
    // argv[4] number of writers
    int sem_id, status=0, ShmID, i, n, j, peers_num, entries_num;
    struct shmid_ds Myshmid_ds;
    pid_t pid;
    int rdrs_num=5, wrtrs_num=5;
    int sem_key;
    int isRdr_Wrtr; // 1 for reader 0 for writer
    key_t key;
    int ShM_id;//, Writer_id, Reader_id, Mutex_id, Printer_id;
    Entry ShMPtr;   // ShMData *ShMPtr;
    FILE *temp_file;

    srand(time(NULL));

    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //check line input
    if (argc != 3) {
		fprintf(stderr, "Usage: need more arguments!\n");
        exit(0);
    }
    temp_file = fopen("temp_file.txt", "w");
    peers_num=atoi(argv[1]);
    entries_num=atoi(argv[2]);
    fprintf(temp_file, "Num of peers: %d and num of entries: %d.\n", peers_num, entries_num);
    // fprintf(temp_file, "Initial array of entry values is:\n");
    int ar_entry[entries_num];
    for(i=0; i<entries_num; i++){
        ar_entry[i]=i+1;
        // fprintf(temp_file,"%d\n", ar_entry[i]);
    }
    // fclose(temp_file);
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    key = ftok("coordinator.c", getppid());
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
        ShMPtr[i].semr = Sem_Init( (key_t)sem_key, 1, 1 );
        sem_key++;
        ShMPtr[i].value = ar_entry[i];
        fprintf(temp_file,"Entry %d:\n\tvalue = %d\n\tsemaphore key = %d\n", i+1, ShMPtr[i].value, ShMPtr[i].semr);
    }
    fprintf(temp_file, "\n");
    fclose(temp_file);
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //fot_childs
    pid_t pids[peers_num];
    /* Start children. */
    for (i = 0; i < peers_num; ++i) {
        // isRdr_Wrtr = read_write(&rdrs_num, &wrtrs_num);
        isRdr_Wrtr = read_or_write( 0.6, 0.4 );
        pids[i] = fork();
        if ( pids[i] < 0 ) {
            fprintf(stderr, "Fork failed.\n");
			exit(EXIT_FAILURE);
        }
        else if (pids[i] == 0) {
            print_whoami("Child");
            for(j=0; j<4; j++){
                temp_file = fopen("temp_file.txt", "a");
                proc_func(isRdr_Wrtr, ShMPtr, entries_num, temp_file);
                fclose(temp_file);
            }
            exit(0);
        }
    }
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    /* Wait for children to exit. */
    n=peers_num;
    while (n > 0) {
        // printf("Before wait getpid %d\n",getpid());
        pid = wait(&status);
        // printf("After wait getpid %d\n",getpid());
        // printf("After wait pid %d\n",pid);
        printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
        --n;  //  Remove pid from the pids array.
    }
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    for(i=0; i<entries_num; i++){
        printf("%d\n", ShMPtr[i].value);
        Sem_Del(ShMPtr[i].semr);
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
    // fprintf(temp_file, "\n");
    // fclose(temp_file);
    // remove("temp_file.txt");

    return 0;
}
