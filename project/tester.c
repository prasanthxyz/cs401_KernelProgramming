#include <stdio.h>
#include "pbarrier.h"

int main()
{
    int i;
    for(i = 0; i < 7; i++)
    {
        printf("%d\n", i);
        if(i == 3)
            block();
    }
    printf("\n");
}
