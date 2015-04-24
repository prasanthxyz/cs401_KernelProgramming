#include <stdio.h>
#include <stdlib.h>
#include "pbarrier.h"

int main(int argc, char *argv[])
{
    int opn, arg, temp;
    if(argc != 3) {
        printf("Enter operation and argument\n");
        scanf(" %d %d", &opn, &arg);
    } else {
        opn = atoi(argv[1]);
        arg = atoi(argv[2]);
    }
    printf("%d", getpid());
    temp = pbarrier(opn, arg);
    //printf("%d\n", temp);
    return 0;
}
