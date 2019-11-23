// getpid(): obtain my own ID
// getppid(): get the ID of my parent
#include <sys/wait.h>
#include <sys/sem.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/shm.h>

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

static int set_semvalue(void);
static void del_semvalue(void);
static int semaphore_p(void);
static int semaphore_v(void);
static int sem_id;

/* Number of elements in shared memory buffer */
#define NUM_ELEM 10

void checker(const char *whoami){
        printf("I am a %s. My pid: %d, my ppid: %d\n", whoami, getpid(), getppid() );
}

int main(int argc, char** argv){
    // argv[1] is number of peers
    // argv[2] is number of entries
    int status;
    int ShmID;
    int n;
    struct shmid_ds Myshmid_ds;
    pid_t pid;

    if (argc != 3) {
		fprintf(stderr, "Usage: need more arguments!\n");
        exit(0);
    }
    int peers_num=atoi(argv[1]);
    int entries_num=atoi(argv[2]);
    printf("Parameters argc: %d, num of peers: %d and num of entries: %d.\n", argc, peers_num, entries_num);
    
    for(int i=0; i<argc; i++){
        printf("For i %d argv[i] %s\n", i, argv[i]);
    }
    
    int ar_entry[entries_num];
    for(int i=0; i<entries_num; i++){
        ar_entry[i]=i+1;
    }

    int i;
	int pause_time;
	char op_char = 'O';
	srand((unsigned int)getpid());
	sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);
	if (argc > 1) {
		if (!set_semvalue()) {
			fprintf(stderr, "Failed to initialize semaphore\n");
			exit(EXIT_FAILURE);
		}
		op_char = 'X';
		sleep(2);
	}

    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //

    /* Init Shared memory */
    // ShmID=shmget(IPC_PRIVATE, NUM_ELEM, 0666 | IPC_CREAT); 
    
    pid_t pids[peers_num];
    
    checker("parent");
    /* Start children. */
    for (int i = 0; i < peers_num; ++i) {

        if ( (pids[i] = fork()) < 0 ) {
            perror("fork failed");
			exit(1);
        }
        else if (pids[i] == 0) {
            checker("child");
            exit(0);
        }

    }

    /* Wait for children to exit. */
    n=peers_num;
    while (n > 0) {
        pid = wait(&status);
        printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
        --n;  // TODO(pts): Remove pid from the pids array.
    }

    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    // // // // // // // // // // // // // // // // // // // // // // // // //
    for(i = 0; i < 10; i++) {
		if (!semaphore_p()){
            exit(EXIT_FAILURE);
        }
		printf("%c", op_char);fflush(stdout);
		pause_time = rand() % 3;
		sleep(pause_time);
		printf("%c", op_char);fflush(stdout);
		if (!semaphore_v()){
            exit(EXIT_FAILURE);
        }
        pause_time = rand() % 2;
        sleep(pause_time);
	}
    printf("\n%d - finished\n", getpid());
    if (argc > 1) {
        sleep(10);
        del_semvalue();
    }
    /* Remove shared memory */
    // shmctl(ShmID, IPC_RMID, &Myshmid_ds);

    return 0;
}





static int set_semvalue(void)
{
	union semun sem_union;
	sem_union.val = 1;
	if (semctl(sem_id, 0, SETVAL, sem_union) == -1){
        return(0);
    }
	return(1);
}

static void del_semvalue(void)
{
	union semun sem_union;
	if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1){
	    fprintf(stderr, "Failed to delete semaphore\n");
    }
}

static int semaphore_p(void)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1; /* P() */
	sem_b.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr, "semaphore_p failed\n");
		return(0);
	}
	return(1);
}

static int semaphore_v(void){
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1; /* V() */
	sem_b.sem_flg = SEM_UNDO;

	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr, "semaphore_v failed\n");
		return(0);
	}

	return(1);
}
