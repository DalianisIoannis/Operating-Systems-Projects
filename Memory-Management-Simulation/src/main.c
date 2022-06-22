#include "../headers/functions.h"

int main(int argc, char** argv){
    // argv[1] is the page replacement LRU or WS
    // argv[2] is the number of IPT frames
    // argv[3] is the number q of lines read from each trace
    // argv[4] is the total traces read
    // argv[5] is WS windows if i choose WS
    if(argc<5){
        fprintf(stderr, "Usage: need at least 4 arguments!\n");
        return -1;
    }
    char* repl_alg = malloc( (strlen(argv[1])+1) * sizeof(char) );
    strcpy(repl_alg, argv[1]);
    if( strncmp(repl_alg,"LRU", 3)!=0 && strncmp(repl_alg,"WS", 2)!=0 ){
        fprintf(stderr, "Usage: First argument must be WS or LRU!\n");
        return -1;
    }
    int frames              = atoi(argv[2]);
    int quantum             = atoi(argv[3]);
    int total_references    = atoi(argv[4]);;
    int window_size         = 0;
    if(strcmp(repl_alg,"WS")==0){
        if(argc<6){
            fprintf(stderr, "Usage: WS needs window size!\n");
            free(repl_alg);        
            return -1;
        }
        else{
            window_size = atoi(argv[5]);
            if(window_size<=0){
                fprintf(stderr, "Usage: no argument can be <=0!\n");
                free(repl_alg);        
                return -1;
            }
        }
    }
    if(frames<=0 || quantum<=0 || total_references<=0){
        fprintf(stderr, "Usage: no argument can be <=0!\n");
        free(repl_alg);        
        return -1;
    }
    virtual_memory(repl_alg, frames, quantum,total_references, window_size);

    free(repl_alg);
    return 0;
}