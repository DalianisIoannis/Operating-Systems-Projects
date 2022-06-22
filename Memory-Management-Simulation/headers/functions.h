#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FRAMESIZE 4096

typedef struct mem_addr{
    long    serial_number;  // how early did it enter. The smallest num is the earliest
    int     processId;
    int     pageNumber;
    int     dirty;          // bool
    char*   op;             // W or R
} Address;


typedef struct InvertedPageTable{
    int         frames;
    int         current_frames; // how many frames are used at the moment
    Address**   Addresses;
} IPT;


typedef struct Working_Set_entry{
    int     pageNumber;
    long    serial_number;
} WSentry;


typedef struct working_set{
    int         current_windows;
    int         window_size;
    WSentry**   entry;
} Wrk_Set;


typedef struct statistics{
    long pageRequests;      // all requests regardless if the page is in IPT
    long pageFaults;        // pageNumber not in IPT
    long pagesLoaded;       // pages loaded back to disk
    long pagesFound;        // pages fetched found in IPT
    long pagesReplaced;     // had to replace page from IPT due to lack of space for newly fetched page
    long pagesRead;
    long pagesWrite; 
} Stats;


int virtual_memory(const char*, const int, const int, const int, const int);

void LRU(int, int, IPT*, long*, Address**, Stats*);

void WS(int, int, IPT*, long*, Address**, Wrk_Set*, long*, Stats*);

void WSet_Inserts(int, Wrk_Set*, Address**, long*, int);

int is_pageNum_in_WS(IPT*, const int, Wrk_Set*, int*);

void InsertWSet(const int, Wrk_Set*, Address**, long*);

void RemoveWSet(const int, Wrk_Set*, Address**, long*);

void InsertIPT(const int, IPT* Table, Address**, long*);

void RemoveIPT(const int, IPT*);

Wrk_Set* initialWSet(const int);

void freeWSet(Wrk_Set*);

IPT* IPTinit(const int);

void freeIPT(IPT*);

void printIndex(IPT*, Wrk_Set*, Wrk_Set*, const char*);

Address** initAdr(const int , const int, const char*);

int is_pageNum_in_IPT(IPT*, int*, Address**);

int min_priorityIPT(IPT*, const int);

void printStatistics(Stats*);

Stats* stats_init();

void freeAdr(Address**);