#include <stdio.h>
#include "pbarrier.h"

int main()
{
    int opn, arg, temp;
    scanf(" %d %d", &opn, &arg);
  //  temp = syscall(322, opn, arg);
    temp = pbarrier(opn, arg);
    printf("%d\n", temp);
    return 0;
}
