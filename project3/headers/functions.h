#pragma once

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "./ShMFuncs.h"

void print_whoami(int id);

int feeder(int, int);

Entry initSharedMem(int, int);

void fnExit (int, Entry);