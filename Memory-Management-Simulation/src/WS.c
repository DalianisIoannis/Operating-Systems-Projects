#include "../headers/functions.h"

Wrk_Set* initialWSet(const int window_size){
    Wrk_Set* WSet_pid = malloc( sizeof(Wrk_Set) );
    if(WSet_pid==NULL){
        fprintf(stderr, "Didn't allocate WSet array.\n");
        exit(-1);
    }
    WSet_pid->window_size       = window_size;
    WSet_pid->current_windows   = 0;
    WSet_pid->entry             = malloc( window_size*sizeof(WSentry*) );
    if(WSet_pid->entry==NULL){
        fprintf(stderr, "Didn't allocate WSet_pid Address array.\n");
        exit(-1);
    }
    for(int i=0; i<window_size; i++){
        WSet_pid->entry[i] = malloc( sizeof(WSentry) );
        if(WSet_pid->entry[i]==NULL){
            fprintf(stderr, "Didn't allocate WSet_pid Address.\n");
            exit(-1);
        }
        WSet_pid->entry[i]->pageNumber      = -1;
        WSet_pid->entry[i]->serial_number   = -1;
    }
    return WSet_pid;
}

void freeWSet(Wrk_Set* WSet){
    for(int i=0; i<WSet->window_size; i++){
        free(WSet->entry[i]);
    }
    free(WSet->entry);
    free(WSet);
}

int min_priorityWSet(Wrk_Set* WSet){    // returns position in WSet with min priority
    int rep_wind = 0, j = 1;        // if find empty returns empty
    long min_ser = WSet->entry[0]->serial_number;
    while(j<WSet->window_size){
        if(WSet->entry[j]->serial_number < min_ser){
            min_ser = WSet->entry[j]->serial_number;
            rep_wind = j;
        }
        j++;
    }
    return rep_wind;
}

int is_pageNum_in_WS(IPT* Table, const int table_frame, Wrk_Set* WSet, int* empty_window){
    int k=0;        // return WSet first empty and k indicates the window the while exited
    while(k < WSet->window_size){                               // where to put new address-does the pageNum already exist
        if(WSet->entry[k]->pageNumber == -1 && (*empty_window) == -1){     // first empty frame if exists
            (*empty_window) = k;
        }
        if(WSet->entry[k]->pageNumber == Table->Addresses[table_frame]->pageNumber){    // found pageNumber in WSet
            break;
        }
        k++;                                                    // if i don't find pageNumber k returns equal to frames
    }
    return k;
}

void InsertWSet(const int window_place, Wrk_Set* WSet, Address** adr, long* WSserial_num){
    WSet->entry[window_place]->pageNumber       = (*adr)->pageNumber;
    WSet->entry[window_place]->serial_number    = (*WSserial_num)++;
    if(WSet->current_windows < WSet->window_size){
        WSet->current_windows++;
    }
}

void RemoveWSet(const int window_place, Wrk_Set* WSet, Address** adr, long* WSserial_num){
    WSet->entry[window_place]->pageNumber       = -1;
    WSet->entry[window_place]->serial_number    = -1;
    WSet->current_windows--;
}

void WSet_Inserts(int window_pos, Wrk_Set* WSet, Address** adr, long* WSserial_num, int empty_window){
    int rep_wind, j;
    if(window_pos >= WSet->window_size){                    // pageNumber not in WSet
        if(WSet->window_size == WSet->current_windows){     // all inserts have been made
            rep_wind = min_priorityWSet(WSet);  // find smallest signature at place rep_wind
            InsertWSet(rep_wind, WSet, adr, WSserial_num);
        }
        else{                                                   // not all inserts have been made
            InsertWSet(empty_window, WSet, adr, WSserial_num);  // first empty window is given with empty_window
        }
    }
    else{                       	                        // pageNumber already in WSet
        if(WSet->window_size == WSet->current_windows){     // all inserts have been made
            rep_wind = min_priorityWSet(WSet);                  // find smallest signature at place rep_wind
            RemoveWSet(rep_wind, WSet, adr, WSserial_num);
            j = 0;                      // is pageNumber in WSet?
            while(j<WSet->window_size){
                if(j!=rep_wind && WSet->entry[j]->pageNumber == (*adr)->pageNumber){    // rep_wind place of first empty
                    break;
                }
                j++;
            }
            if(j>=WSet->window_size){   // have not found
                InsertWSet(rep_wind, WSet, adr, WSserial_num);
            }// else{}                      // does not insert
        }
        else{                           // not all inserts have been made
            WSet->entry[window_pos]->serial_number = (*WSserial_num)++; // change ser_num at pageNum position
        }
    }
    return;
}

void WS(int frame_pos, int isEmpty, IPT* Table, long* IPTserial_num, Address** adr, Wrk_Set* WSet, long* WSserial_num, Stats* statisticsInfo){
    int empty_window, k, j, empty_frame, deleted, del, remover;
    if(Table->frames != Table->current_frames){ // not full IPT
        if(Table->frames <= frame_pos){         // pageNumber not in IPT
            statisticsInfo->pageFaults++;
            InsertIPT(isEmpty, Table, adr, IPTserial_num); // isEmpty has the first empty frame
            remover = isEmpty;
        }
        else{                                   // pageNumber in IPT
            statisticsInfo->pagesFound++;
            Table->Addresses[frame_pos]->serial_number = (*IPTserial_num)++;
            remover = frame_pos;
        }
        empty_window = -1;      // check WS find if pageNumber is in Working Set
        k = is_pageNum_in_WS(Table, remover, WSet, &empty_window);
        WSet_Inserts(k, WSet, adr, WSserial_num, empty_window);
    }
    else{                           // IPT is full
        if(Table->frames <= frame_pos){         // pageNumber not in IPT
            statisticsInfo->pageFaults++;

            deleted = 0;                // delete IPT pages not in WS
            for(k=0; k<Table->frames; k++){
                del = 0;
                for(j=0; j<WSet->window_size; j++){
                    if( (Table->Addresses[k]->pageNumber == WSet->entry[j]->pageNumber && Table->Addresses[k]->processId==(*adr)->processId) || Table->Addresses[k]->processId!=(*adr)->processId){
                        del = 1;        // found in WS
                        break;
                    }
                }
                if(del==0){             // one deletes another's
                    if(Table->Addresses[k]->dirty==1){
                        statisticsInfo->pagesLoaded++;
                    }
                    RemoveIPT(k, Table);
                    deleted++;      // how many pages removed
                }
            }

            if(deleted==0){ // didn't remove any IPT still full
                empty_frame = min_priorityIPT(Table, (*adr)->processId);
                RemoveIPT(empty_frame, Table);
                statisticsInfo->pagesReplaced++;
                InsertIPT(empty_frame, Table, adr, IPTserial_num);
            }
            else{
                k = 0;
                empty_frame = -1;
                while(k<Table->frames){
                    if(Table->Addresses[k]->pageNumber==-1 && empty_frame==-1){
                        empty_frame = k;    //first empty
                    }
                    if(Table->Addresses[k]->pageNumber==(*adr)->pageNumber && Table->Addresses[k]->processId==(*adr)->processId){
                        break;
                    }
                    k++;
                }
                if(k>=Table->frames){   // didn't find it
                    InsertIPT(empty_frame, Table, adr, IPTserial_num);  //insert in empty
                }
            }
        }
        else{           // pageNum in IPT
            statisticsInfo->pagesFound++;
            empty_frame = frame_pos;    // frame_pos is where found
            Table->Addresses[frame_pos]->serial_number = (*IPTserial_num)++;
            deleted = 0;                // delete IPT pages not in WS
            for(k=0; k<Table->frames; k++){
                del = 0;    // dont delete the one we found
                if(Table->Addresses[k]->pageNumber != Table->Addresses[frame_pos]->pageNumber){
                    for(j=0; j<WSet->window_size; j++){
                        if( (Table->Addresses[k]->pageNumber == WSet->entry[j]->pageNumber && Table->Addresses[k]->processId==(*adr)->processId) || Table->Addresses[k]->processId!=(*adr)->processId){
                            del = 1;        // found in WS
                            break;
                        }
                    }
                    if(del==0){             // one deletes another's
                        if(Table->Addresses[k]->dirty==1){
                            statisticsInfo->pagesLoaded++;
                        }
                        RemoveIPT(k, Table);
                        deleted++;      // how many pages removed
                    }
                }
            }
        }
        statisticsInfo->pagesReplaced+=deleted;
        empty_window = -1;  // WS find if pageNumber is in Working Set
        k = is_pageNum_in_WS(Table, empty_frame, WSet, &empty_window);
        WSet_Inserts(k, WSet, adr, WSserial_num, empty_window);
    }
    return;
}