#define _GNU_SOURCE
#include "../headers/functions.h"

void printIndex(IPT* InvTable, Wrk_Set* WSet_pid1, Wrk_Set* WSet_pid2, const char* repl_alg){
    int i;
    printf("IPT frames:\n");
    for(i=0; i<InvTable->frames; i++){
        printf("\tEntrance %d has pageNumber %d, pid %d, priority %ld and operation %s.\n", i, InvTable->Addresses[i]->pageNumber, InvTable->Addresses[i]->processId, InvTable->Addresses[i]->serial_number, InvTable->Addresses[i]->op);
    }
    if(strcmp(repl_alg, "WS")==0){
        printf("WSet_pid1 windows:\n");
        for(i=0; i<WSet_pid1->window_size; i++){
            printf("\tEntrance %d has pageNumber %d and priority %ld.\n", i, WSet_pid1->entry[i]->pageNumber, WSet_pid1->entry[i]->serial_number);
        }
        printf("WSet_pid2 windows:\n");
        for(i=0; i<WSet_pid2->window_size; i++){
            printf("\tEntrance %d has pageNumber %d and priority %ld.\n", i, WSet_pid2->entry[i]->pageNumber, WSet_pid2->entry[i]->serial_number);
        }
    }
}

int virtual_memory(const char* repl_alg, const int frames, const int quantum, const int total_references, const int window_size){
    long        decAddr, IPT_serial_num = 0, WS_serial_num = 0;         
    int         i, num_of_address_read = 0, pageNumber, empty_frame, pid, temp;
    char        *pageMod, *hexAdr, *token, *line = NULL, *line1 = NULL, *line2 = NULL;
    Address**   adr;
    size_t      len = 0;
    ssize_t     read;
    FILE        *file1, *file2, *file_ref;
    IPT*        InvTable;
    Wrk_Set     *WSet_pid1, *WSet_pid2;
    Stats*      statisticsInfo = stats_init();
    // // // // // // // // // // // // // // // open traces
    file1 = fopen("traces/bzip.trace", "r");
    file2 = fopen("traces/gcc.trace", "r");
    if(file1==NULL || file2==NULL){
        fprintf(stderr, "Didn't open trace.\n");
        return -1;
    }
    // // // // // // // // // // // // // // // read file lines
    srand(time(NULL));      // randomly choose first trace
    if( rand() % 2 + 0 ==0 ){
        printf("\nStarting with file bzip.trace.\n");
        file_ref = file1;
        line = line1;
        pid = 1;    // bzip.trace is pid 1
    }
    else{
        printf("\nStarting with file gcc.trace.\n");
        file_ref = file2;
        line = line2;
        pid = 2;    // gcc.trace is pid 2
    }
    // // // // // // // // // // // // // IPT and Working Set Initialization
    InvTable    = IPTinit(frames);
    WSet_pid1   = initialWSet(window_size);
    WSet_pid2   = initialWSet(window_size);
    // // // // // // // // // // // // // // // Implementation
    read = getline( &line, &len, file_ref );
    temp = 0;
    while( read != -1 && num_of_address_read<total_references ){
        // printf("File line is %s", line);
        token   = strtok(line, " \t");
        hexAdr  = malloc( (strlen(token)+1)*sizeof(char) );
        strcpy(hexAdr, token);                  // address in Hex
        decAddr = strtoul(hexAdr, NULL, 16);    // address in dec
        pageNumber = decAddr / FRAMESIZE;
        token   = strtok(NULL, " \t\n");
        pageMod = malloc( (strlen(token)+1)*sizeof(char) );
        strcpy(pageMod, token);                 // pageMod is operation W or R
        // printf("hexAdr %s and pageMod %s\n", hexAdr, pageMod);
        printf("Address in decimal is %ld, pageNumber %d, operation %s and pid %d\n", decAddr, pageNumber,pageMod ,pid);

        adr = initAdr(pageNumber, pid, pageMod);

        statisticsInfo->pageRequests++;
        if( strcmp( (*adr)->op, "R")==0 ){
            statisticsInfo->pagesRead++;
        }
        else{
            statisticsInfo->pagesWrite++;
        }

        empty_frame = -1;
        i = is_pageNum_in_IPT(InvTable, &empty_frame, adr); // if i >= frames, pageNumber hasn't been found 
        if(strcmp(repl_alg, "LRU")==0){
            LRU(i, empty_frame, InvTable, &IPT_serial_num, adr, statisticsInfo);
        }
        else{
            if(pid==1){
                WS(i, empty_frame, InvTable, &IPT_serial_num, adr, WSet_pid1, &WS_serial_num, statisticsInfo);
            }
            else{
                WS(i, empty_frame, InvTable, &IPT_serial_num, adr, WSet_pid2, &WS_serial_num, statisticsInfo);
            }
        }
        freeAdr(adr);

        printf("\nAfter repeat %d:\n", num_of_address_read);
        printIndex(InvTable, WSet_pid1, WSet_pid2, repl_alg);
        printStatistics(statisticsInfo);

        free(pageMod);
        free(hexAdr);
        num_of_address_read++;

        if(++temp==quantum){
            temp = 0;
            free(line);
            if(file_ref==file1){
                file_ref = file2;
                line = line2;
                pid = 2;
            }
            else{
                file_ref = file1;
                line = line1;
                pid = 1;
            }
        }

        if(num_of_address_read<total_references){   // read next line only if haven't reached limit
            read = getline( &line, &len, file_ref );
        }
    }
    printf("\n\nFINISHED\n");
    printIndex(InvTable, WSet_pid1, WSet_pid2, repl_alg);
    printStatistics(statisticsInfo);
    printf("\n");
    freeIPT(InvTable);
    freeWSet(WSet_pid1);
    freeWSet(WSet_pid2);
    free(statisticsInfo);
    free(line);
    fclose(file1);
    fclose(file2);
    return 0;
}