#include <stdio.h>
#include <stdlib.h>
#include "pbarrier.h"

int main(int argc, char *argv[])
{
    int opn, arg1, arg2, temp;
    if(argc != 4) {
        printf("Enter operation and arguments\n");
        scanf(" %d %d %d", &opn, &arg1, &arg2);
    } else {
        opn = atoi(argv[1]);
        arg1 = atoi(argv[2]);
        arg2 = atoi(argv[3]);
    }
    printf("%d", getpid());
    temp = pbarrier(opn, arg1, arg2);
    //printf("%d\n", temp);
    return 0;
}
