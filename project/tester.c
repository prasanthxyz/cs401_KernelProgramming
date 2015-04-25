#include <stdio.h>
#include <stdlib.h>
#include "pbarrier.h"

int main(int argc, char* argv[])
{
    int i, gid = -1, nproc = -1;
    if(argc == 2) {
        gid = atoi(argv[1]);
    } else if(argc == 3) {
        gid = atoi(argv[1]);
        nproc = atoi(argv[2]);
    }

    if(nproc != -1) {
        initGroup(gid, nproc);
    }

    printf("Going to print from 0 to 10, calling block after 3\n");
    for(i = 0; i < 7; i++)
    {
        printf("\n%d", i);
        if(i == 3) {
            printf("\nCalling block");
            block(gid);
            printf("\nOut of block");
        }
    }
    printf("\n");
    return 0;
}
