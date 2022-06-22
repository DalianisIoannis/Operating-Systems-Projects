#include "../headers/functions.h"

Address** initAdr(const int pageNumber, const int pid, const char* pageMod){
    Address** adr = malloc(sizeof(Address*));   // make address with the items taken    
    (*adr)  = malloc( sizeof(Address) );
    (*adr)->pageNumber  = pageNumber;
    (*adr)->processId   = pid;
    (*adr)->op          = malloc( (strlen(pageMod)+1) * sizeof(char) );
    strcpy( (*adr)->op, pageMod );
    return adr;
}

void freeAdr(Address** adr){
    free( (*adr)->op );
    free( (*adr) );
    free(adr);
    return;
}