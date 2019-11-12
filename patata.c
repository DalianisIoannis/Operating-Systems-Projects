#include<sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>


int main(void){
    pid_t pids[10];
    int i;
    int n = 10;

    /* Start children. */
    for (i = 0; i < n; ++i) {
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
}