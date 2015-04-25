#include <stdio.h>
#include <stdlib.h>
#include "pbarrier.h"

int main(int argc, char *argv[])
{
    int nproc;
    if(argc != 2) {
        printf("Enter the number of processes: ");
        scanf(" %d", &nproc);
    } else {
        nproc = atoi(argv[1]);
    }
    pbarrier(2, nproc);
    return 0;
}
