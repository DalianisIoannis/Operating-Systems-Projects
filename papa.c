#include<sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void menu(long, long);
int evaluateMenu(int, long);

int main() {
	
	long ppid = getppid();
    long pid = getpid();
    printf("\nParent Process ID: %ld, Parent's parent process ID: %ld \n",pid,ppid);
	while(getppid() == ppid){
		menu(ppid, pid);
		wait(NULL);  /* wait for response from child processes */
	}

   return 0;
}

/* Function to print menu options. This 
is the main menu function to control user 
input options including cases 1-4. If the
user enters 4 the program will exit() 
terminating all processes. */
void menu(long ppid, long pid) {
    
    pid = getpid(); /* get main menu pid */
	printf("Choose the function to be performed by the child:\n"
	"(1) Display the current date and time.\n"
	"(2) Display the calendar of the current month.\n"
	"(3) List the files in the current directory.\n"
	"(4) Exit the program.\n\n"
	"Enter your choice: \n");
    int choice = 0;
	scanf("%d", &choice);
	if(choice == 4) {
		printf("Exiting Program....\n");
		/* terminates program and all processes */
		exit(0);
	}
	evaluateMenu(choice, pid);
 
}

/* Function to evaluate the choice the user makes when 
selecting the case to perform. Includes creation of Child
processes as well as process ID identification/management */
int evaluateMenu(int choice, long pid){
	
	long new_pid;
	
	switch (choice) {
		case 1:
			new_pid = fork();
			if (new_pid == 0) {
                printf("Child process ID %d, Parent ID %ld \n\n", getpid(),pid);
                system("TZ=America/New_York date");
                printf("\n");
                printf("Current process ID %ld\n", pid);
			} 
			break;
		case 2:
			new_pid = fork();
			if (new_pid == 0) {
                printf("Child process ID %d, Parent ID %ld \n\n", getpid(),pid);
                system("cal");
                printf("\n");
                printf("Current process ID %ld\n", pid);
			}
			break;
		case 3:
			new_pid = fork();
			if(new_pid == 0) {
                printf("Child process ID %d, Parent ID %ld \n\n", getpid(),pid);
                system("ls -a -l");
                printf("\n");
                printf("Current process ID %ld\n", pid);
			}
			break;
		default:
			printf("Error, please select a choice between 1 and 4!\n\n");
			break;	
	}
	
}