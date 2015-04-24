#include <stdio.h>
#include "pbarrier.h"

int main()
{
    int i;
    initGroup(1, 4);
    for(i = 0; i < 7; i++)
    {
        printf("%d\n", i);
        if(i == 3)
            block(1);
    }
    printf("\n");
}
