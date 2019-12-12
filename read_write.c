#include "functions.h"

int read_or_write(float read_per, float wrt_per){
    float a = 1.0;
    float x = (float)rand() / (float)(RAND_MAX/a);
    if(x<=read_per){
        return 1;   //reader
    }
    else{
        return 0;   //writer
    }
}

double proc_func(int isrd_wrt, Entry mentry, int entrs, FILE* temp_file){
    // exponential time
    double T = MULTIPLIER*( -log((double)rand()/(double) RAND_MAX) / LEXP  );
    struct timespec starter, ender;
    double time_taken;
    int rand_entr = rand()%entrs;
    int sem_first_read, sm_rd;
    int random_int = rand()%100+1;

    if(isrd_wrt==1){//is reader

        sm_rd = Sem_Down( mentry[rand_entr].mutex, 0 );
        if(sm_rd<0){
            fprintf(stderr, "Faiure in read process.\n");
            return -1;
        }

        mentry[rand_entr].read_count++;
        if( mentry[rand_entr].read_count==1 ){
            sem_first_read = Sem_Down( mentry[rand_entr].rw_mutex, 0 );
            if(sem_first_read<0){
                fprintf(stderr, "Faiure in read process.\n");
                return -1;
            }
        }

        sm_rd = Sem_Up( mentry[rand_entr].mutex, 0 );
        if(sm_rd<0){
            fprintf(stderr, "Failure in read process.\n");
            return -1;
        }
        
        clock_gettime(CLOCK_MONOTONIC, &starter);

        sm_rd = Sem_Down( mentry[rand_entr].mutex, 0 );
        if(sm_rd<0){
            fprintf(stderr, "Faiure in read process.\n");
            return -1;
        }

        clock_gettime(CLOCK_MONOTONIC, &ender);
        time_taken = (ender.tv_sec - starter.tv_sec) * 1e9;
        time_taken = (time_taken + (ender.tv_nsec - starter.tv_nsec)) * 1e-9;

        /* reading */
        sleep(T);
        fprintf(temp_file, "Read entry %d with value = %d with process pid %d\n",rand_entr+1, mentry[rand_entr].value, getpid());
        mentry[rand_entr].reads_made++;

        mentry[rand_entr].read_count--;
        if( mentry[rand_entr].read_count==0 ){
            sem_first_read = Sem_Up( mentry[rand_entr].rw_mutex, 0 );
            if(sem_first_read<0){
                fprintf(stderr, "Faiure in read process.\n");
                return -1;
            }
        }

        sm_rd = Sem_Up( mentry[rand_entr].mutex, 0 );
        if(sm_rd<0){
            fprintf(stderr, "Faiure in read process.\n");
            return -1;
        }
        return time_taken;
    }

    else{//is writer
        
        clock_gettime(CLOCK_MONOTONIC, &starter);

        int sm_wrt = Sem_Down( mentry[rand_entr].rw_mutex, 0 );
        if(sm_wrt<0){
            fprintf(stderr, "Faiure in write process.\n");
            return -1;
        }

        clock_gettime(CLOCK_MONOTONIC, &ender);
        time_taken = (ender.tv_sec - starter.tv_sec) * 1e9;
        time_taken = (time_taken + (ender.tv_nsec - starter.tv_nsec)) * 1e-9;

        sleep(T);
        fprintf(temp_file, "Write %d at entry %d with process pid %d\n",random_int  ,rand_entr+1, getpid());
        mentry[rand_entr].value = random_int;
        mentry[rand_entr].writes_made++;

        sm_wrt = Sem_Up( mentry[rand_entr].rw_mutex, 0 );
        if(sm_wrt<0){
            fprintf(stderr, "Faiure in write process.\n");
            return -1;
        }
    }
    return time_taken;
}

void print_whoami(int id){
        if( getppid()!=id ){
            printf("Child created with pid: %d and ppid: %d.\n", getpid(), getppid() );
        }
        else{
            printf("This is parent process with id: %d and ppid: %d.\n", getpid(), getppid());
        }
}