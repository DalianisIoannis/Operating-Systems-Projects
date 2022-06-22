#include "../headers/functions.h"

void LRU(int frame_pos, int isEmpty, IPT* Table, long* ser_num, Address** adr, Stats* statisticsInfo){
    int min_pos;
    if( frame_pos==Table->frames ){                             // frame has not been found
        statisticsInfo->pageFaults++;
        if(isEmpty!=-1){                                        // there is empty
            InsertIPT(isEmpty, Table, adr, ser_num);
        }
        else{                                                   // there is no empty frame
            min_pos = min_priorityIPT(Table, -1);
            if(Table->Addresses[min_pos]->dirty==1){
                statisticsInfo->pagesLoaded++;
            }
            statisticsInfo->pagesReplaced++;
            RemoveIPT(min_pos, Table);
            InsertIPT(min_pos, Table, adr, ser_num);
        }
    }
    else{                                                       // frame has been found
        Table->Addresses[frame_pos]->serial_number = (*ser_num)++;
        statisticsInfo->pagesFound++;
    }
    return;
}