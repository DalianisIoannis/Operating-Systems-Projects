#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv){

    // argv[0] is number of entries 
    int nentr=atoi(argv[1]);
    printf("Parameters given argc %d and num of entries is %d.\n", argc, nentr);
    for(int i=0; i<argc; i++){
        printf("For i %d argv[i] %s\n", i, argv[i]);
    }
    int* ent_ar;
    ent_ar=malloc(nentr*sizeof(int));
    for(int i=0; i<nentr; i++){
        ent_ar[i]=i;
        printf("for i=%d the value is %d\n",i, ent_ar[i]);
    }
    
    // pid_t pid;
    pid_t pids[nentr];

    // pid=0;
    // printf("pid after init is %d\n",pid);
    // printf("Fork pid.\n");
    // pid=fork();
    // printf("pid is %d\n",pid);

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
    for (int i = 0; i < nentr; ++i){
        pids[i] = fork();
        // if ( (pids[i] = fork()) < 0 ) {
        if ( pids[i] < 0 ) {
            printf("Fork error.\n");
            perror("fork failed");
            exit(1);
        }
        else if (pids[i] == 0) {
            // DoWorkInChild();
            // exit(0);
        }
    }

// // // // // // // // // // // // // // // // // // 
    /* Wait for children to exit. */
    // int status;
    // pid_t pid;
    // while (n > 0) {
    //     pid = wait(&status);
    //     printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
    //     --n;  // TODO(pts): Remove pid from the pids array.
    // }
// // // // // // // // // // // // // // // // // // 
    // free(pids);
    free(ent_ar);
    return 0;
}