#include "../headers/functions.h"

Stats* stats_init(){
    Stats*      statisticsInfo = malloc( sizeof(Stats) );
    statisticsInfo->pageFaults      = 0;
    statisticsInfo->pageRequests    = 0;
    statisticsInfo->pagesLoaded     = 0;
    statisticsInfo->pagesRead       = 0;
    statisticsInfo->pagesWrite      = 0;
    statisticsInfo->pagesReplaced   = 0;
    statisticsInfo->pagesFound      = 0;
    return statisticsInfo;
}

void printStatistics(Stats* statisticsInfo){
    printf("Simulation Statistics!!\n");
    printf("Page requests %ld.\n", statisticsInfo->pageRequests);
    printf("Page faults %ld.\n", statisticsInfo->pageFaults); 
    printf("Pages loaded back to disk %ld.\n", statisticsInfo->pagesLoaded);
    printf("Pages fetched and found in IPT %ld.\n", statisticsInfo->pagesFound);
    printf("Pages removed from IPT %ld.\n", statisticsInfo->pagesReplaced);
    printf("Reads %ld.\n", statisticsInfo->pagesRead);
    printf("Writes %ld.\n", statisticsInfo->pagesWrite);
    return;
}