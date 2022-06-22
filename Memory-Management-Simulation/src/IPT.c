#include "../headers/functions.h"

void InsertIPT(const int frame_place, IPT* Table, Address** adr, long* IPTserial_num){
    free(Table->Addresses[frame_place]->op);    // insert new address in IPT
    Table->Addresses[frame_place]->op = malloc( (strlen((*adr)->op)+1)*sizeof(char) );
    strcpy(Table->Addresses[frame_place]->op, (*adr)->op);
    if( strcmp(Table->Addresses[frame_place]->op, "W")==0 ){
        Table->Addresses[frame_place]->dirty = 1;
    }
    Table->Addresses[frame_place]->processId        = (*adr)->processId;
    Table->Addresses[frame_place]->pageNumber       = (*adr)->pageNumber;
    Table->Addresses[frame_place]->serial_number    = (*IPTserial_num)++;
    Table->current_frames++;
    return;
}

void RemoveIPT(const int frame_place, IPT* Table){
    free(Table->Addresses[frame_place]->op);
    Table->Addresses[frame_place]->pageNumber       = -1;
    Table->Addresses[frame_place]->processId        = -1;
    Table->Addresses[frame_place]->serial_number    = -1;
    Table->Addresses[frame_place]->op               = NULL; // no string
    Table->current_frames--;
    return;
}

int min_priorityIPT(IPT* Table, const int pid){ // if pid = -1 i don't care for priority
    long min_ser;   // check if new exist and replace
    int rep_wind = 0, k = 0;
    if(pid==-1){
        min_ser = Table->Addresses[0]->serial_number;
        while(k<Table->frames){
            if(Table->Addresses[k]->serial_number < min_ser && Table->Addresses[k]->serial_number != -1){
                min_ser = Table->Addresses[k]->serial_number;
                rep_wind = k;
            }
            k++;
        }
    }
    else{
        while(k<Table->frames){ // find priority of first pid pageNum
            if(Table->Addresses[k]->processId==pid){
                min_ser = Table->Addresses[k]->serial_number;
                break;
            }
            k++;    // min_ser at k
        }
        k++;
        while(k<Table->frames){
            if(Table->Addresses[k]->serial_number < min_ser && Table->Addresses[k]->serial_number != -1 && Table->Addresses[k]->processId==pid){
                min_ser = Table->Addresses[k]->serial_number;
                rep_wind = k;
            }
            k++;
        }
    }
    return rep_wind;
}

IPT* IPTinit(const int frames){
    IPT* InvTable = malloc( sizeof(IPT) );  // initialize IPT
    InvTable->Addresses         = malloc( frames*sizeof(Address*) );    // frames: IPT size
    if(InvTable->Addresses==NULL){
        fprintf(stderr, "Didn't allocate IPT Address array.\n");
        exit(-1);
    }
    InvTable->frames            = frames;
    InvTable->current_frames    = 0;
    for(int i=0; i<frames; i++){
        InvTable->Addresses[i] = malloc( sizeof(Address) );
        if(InvTable->Addresses[i]==NULL){
            fprintf(stderr, "Didn't allocate IPT Address.\n");
            exit(-1);
        }
        InvTable->Addresses[i]->dirty           = -1;
        InvTable->Addresses[i]->processId       = -1;
        InvTable->Addresses[i]->serial_number   = -1;
        InvTable->Addresses[i]->pageNumber      = -1;
        InvTable->Addresses[i]->op              = NULL; // no string
    }
    return InvTable;
}

void freeIPT(IPT* InvTable){
    for(int i=0; i<InvTable->frames; i++){
        if(InvTable->Addresses[i]->pageNumber != -1){
            free(InvTable->Addresses[i]->op);
        }
        free(InvTable->Addresses[i]);
    }
    free(InvTable->Addresses);
    free(InvTable);
    return;
}

int is_pageNum_in_IPT(IPT* InvTable, int* empty_frame, Address** adr){
    int i = 0;  // where adr exists in IPT and first empty
    while(i < InvTable->frames){
        if(InvTable->Addresses[i]->pageNumber == -1 && (*empty_frame) == -1){
            (*empty_frame) = i;
        }
        if(InvTable->Addresses[i]->pageNumber == (*adr)->pageNumber && InvTable->Addresses[i]->processId == (*adr)->processId){
            break;  // found pageNumber in IPT
        }
        i++;    // if i don't find pageNumber i returns equal to frames
    }
    return i;
}