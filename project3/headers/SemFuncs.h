#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
}arg;

int Sem_Init(key_t key, int nsems, int value);

int Sem_Down(int sem_id, int sem_num);  // P() wait sem

int Sem_Up(int sem_id, int sem_num);    // V() signal sem

int Sem_Del(int sem_id);