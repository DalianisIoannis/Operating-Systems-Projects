#include<sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv){
    // argv[1] is number of peers
    // argv[2] is number of entries
    if (argc != 3) {
		fprintf(stderr, "Usage: need more arguments!\n");
        exit(0);
    }
    int peers_num=atoi(argv[1]);
    int entries_num=atoi(argv[2]);
    printf("Parameters given argc %d and num of peers is %d and num of entries is %d.\n", argc, peers_num, entries_num);
    for(int i=0; i<argc; i++){
        printf("For i %d argv[i] %s\n", i, argv[i]);
    }
    int ar_entry[entries_num];
    for(int i=0; i<entries_num; i++){
        ar_entry[i]=i+1;
    }
    // // // // // // // // // // // // // // // // // // // // // // // // // 
    // getpid(): obtain my own ID
    // getppid(): get the ID of my parent
    // printf ( " Process has as ID the number : % ld \n " ,( long ) getpid () );
    // printf ( " Parent of the Process has as ID : % ld \n " ,( long ) getppid () );
    // pid_t pid;
    pid_t pids[peers_num];
    // pid_t pids[3];
    // pid=0;
    // pid=fork();

    // switch(pid){
	// 	case -1:
    //         printf("Fork error.\n");
	// 		perror("fork failed");
	// 		exit(1);
	// 	case 0:
    //         printf("This is the child.\n");
	// 		break;
	// 	default:
    //         printf("This is the parent.\n");
	// 		break;
	// }
    int n = 2;
    /* Start children. */
    for (int i = 0; i < n; ++i) {
        if ((pids[i] = fork()) < 0) {
            perror("fork");
            abort();
        } else if (pids[i] == 0) {
            // DoWorkInChild();
            printf("patata\n");
            exit(0);
        }
    }

    /* Wait for children to exit. */
    int status;
    pid_t pid;
    while (n > 0) {
        pid = wait(&status);
        printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
        --n;  // TODO(pts): Remove pid from the pids array.
    }

    return 0;
}