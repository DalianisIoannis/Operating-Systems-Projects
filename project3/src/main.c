#include "../headers/functions.h"

int main(int argc, char** argv) {
    
    // argv[1] number of processes
    // argv[2] array size

    if (argc != 3) {
		fprintf(stderr, "Usage: need 3 arguments!\n");
        exit(EXIT_FAILURE);
    }

    int proc   = atoi(argv[1]);
    int arSize = atoi(argv[2]);

    if(feeder(proc, arSize)<0) {
        // fprintf(stderr, "Feeder didn't end normally.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}