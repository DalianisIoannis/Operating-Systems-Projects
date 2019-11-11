#include <stdio.h>

int main(int argc, char** argv){
    printf("Parameters given argc %d\n", argc);
    for(int i=0; i<argc; i++){
        printf("For i %d argv[i] %s\n", i, argv[i]);
    }
    return 0;
}