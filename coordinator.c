// getpid(): obtain my own ID
// getppid(): get the ID of my parent
#include <sys/wait.h>
#include <sys/sem.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/shm.h>
#include "sems_shm.h"

/* Number of elements in shared memory buffer */
// #define NUM_ELEM 10

void print_whoami(const char *whoami){
        printf("I am a %s. My pid: %d, my ppid: %d\n", whoami, getpid(), getppid() );
}

int main(int argc, char** argv){
    // argv[1] is number of peers
    // argv[2] is number of entries
    // n = atoi(argv[1]);
    // m = atoi(argv[2]);
    int sem_id, status=0, ShmID, i, n, m, j, peers_num, entries_num;
    int *my_matrix, *cha_array;
    struct shmid_ds Myshmid_ds;
    pid_t pid;
    int pause_time;
    int rdrs_num=5, wrtrs_num=5;

    char isRdr_Wrtr;
    pid_t pidd;
    key_t key;
    int ShM_id, Writer_id, Reader_id, Mutex_id, Printer_id;
    // ShMData *ShMPtr;
    Entry ShMPtr;

    srand(time(NULL));

    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //check line input
    if (argc != 3) {
		fprintf(stderr, "Usage: need more arguments!\n");
        exit(0);
    }
    peers_num=atoi(argv[1]);
    entries_num=atoi(argv[2]);
    printf("Parameters argc: %d, num of peers: %d and num of entries: %d.\n", argc, peers_num, entries_num);
    // for(i=0; i<argc; i++){ printf("For i %d argv[i] %s\n", i, argv[i]); }
    int ar_entry[entries_num];
    for(i=0; i<entries_num; i++){
        ar_entry[i]=i+1;
    }
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    FILE *temp_file = fopen("temp_file.txt", "w");
    // shared memory key
    key = ftok("coordinator.c", 'R');
    // key = ftok("coordinator.c", rand());
    if(key<0){
        fprintf(stderr, "Failed to make key.\n");
        exit(EXIT_FAILURE);
    }
    // have to take num of entries
    ShM_id = ShMInit(key);
    // ShM_id = shmget(key, entries_num*sizeof(Entry), IPC_CREAT | 0666);
    if(ShM_id<0){
        exit(EXIT_FAILURE);
    }
    // ShMPtr = (Entry) shmat(ShM_id, NULL, 0);
    // ShMPtr = (Entry) ShMAttach(ShM_id);
    ShMPtr = ShMAttach(ShM_id);
    if( ShMPtr == (ShMData*)(-1) ){
        fprintf(stderr, "Failed to attach shm.\n");
        exit(EXIT_FAILURE);
    }
    // for(i=0; i<entries_num; i++){
    //     ShMPtr[i].id = 1;
    //     ShMPtr[i].semr = Sem_Init( (key_t)5768, 1, 1 );
    //     ShMPtr[i].value = ar_entry[i];
    // }
    // binary sem nsems=1 and value=1
    Writer_id = Sem_Init( (key_t)1234, 1, 1 );
    if(Writer_id<0){
        exit(EXIT_FAILURE);
    }
    // peers_num number of binary sems
    Reader_id = Sem_Init( (key_t)4321,  peers_num, 0 );
    if(Reader_id<0){
        exit(EXIT_FAILURE);
    }
    // sem for counting
    Mutex_id = Sem_Init( (key_t)2134, 1, 0 );
    if(Mutex_id<0){
        exit(EXIT_FAILURE);
    }
    // binary sem
    Printer_id = Sem_Init( (key_t)3124, 1, 1 );
    if(Printer_id<0){
        exit(EXIT_FAILURE);
    }
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    print_whoami("parent");
    // ar_entry in shm
    for(i=0; i<entries_num; i++){
        // no one else can write
        Sem_Down(Writer_id, 0);
        ShMPtr->value = ar_entry[i];
        // set value to 0
        Sem_Set(Mutex_id, 0, 0);
        for(j=0; j<peers_num; j++){
            // all can now read
            Sem_Up(Reader_id, j);
        }
        // for now needed
        Sem_Up(Writer_id, 0);
    }
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    pid_t pids[peers_num];
    /* Start children. */
    for (i = 0; i < peers_num; ++i) {
        pids[i] = fork();
        if ( pids[i] < 0 ) {
            fprintf(stderr, "Fork failed.\n");
			exit(EXIT_FAILURE);
        }
        else if (pids[i] == 0) {
            print_whoami("child");
            // first write and then read
            cha_array = (int*) malloc(entries_num*sizeof(int));
            for(j=0; j<entries_num; j++){
                // i process wont read the same twice
                Sem_Down(Reader_id, i);
                cha_array[j] = ShMPtr->value;
                // ++counting sem
                Sem_Up(Mutex_id, 0);
                if( Sem_Get(Mutex_id,0)==peers_num ){
                    // free writer when all peers have read
                    Sem_Up(Writer_id, 0);
                }
                // Sem_Up(Reader_id, i);
            }
            // break;
            // WRITE ABOVE
            for(j=0; j<entries_num; j++){
                Sem_Down(Writer_id, 0);
                cha_array[j] = rand()%21;
                ShMPtr->value = cha_array[j];
                // ++counting sem
                Sem_Set(Mutex_id, 0, 0);
                for(j=0; j<peers_num; j++){
                    // all can now read
                    Sem_Up(Reader_id, j);
                }
                // for now needed
                Sem_Up(Writer_id, 0);
            }
        // in child process write 
            printf("In file print\n");
            Sem_Down(Printer_id, 0);
            fprintf(temp_file,"\nChild %d\n",getpid());
            fprintf(temp_file,"The array: \n");
            for(j=0; j<entries_num; j++){
                fprintf(temp_file, "%d\n", cha_array[j]);
            }
            Sem_Up(Printer_id, 0);
            free(cha_array);
            exit(0);
        }
    }
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    /* Wait for children to exit. */
    n=peers_num;
    while (n > 0) {
        printf("Before wait getpid %d\n",getpid());
        pid = wait(&status);
        printf("After wait getpid %d\n",getpid());
        printf("After wait pid %d\n",pid);
        printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
        --n;  // TODO(pts): Remove pid from the pids array.
    }
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    if( ShMDettach(ShMPtr)<0 ){
        fprintf(stderr, "Failed to dettach shm.\n");
        exit(EXIT_FAILURE);
    }
    if( ShMDestroy(ShM_id)<0 ){
        fprintf(stderr, "Failed to destroy shm.\n");
        exit(EXIT_FAILURE);
    }
    Sem_Del(Writer_id);
    Sem_Del(Reader_id);
    Sem_Del(Mutex_id);
    Sem_Del(Printer_id);
    printf("Open text file to check.\n");
    fprintf(temp_file, "\n");
    fclose(temp_file);
    // remove("temp_file.txt");

    return 0;
}
