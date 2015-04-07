#ifndef _PBARRIER_H_
#define _PBARRIER_H_

#define pbarrier(x,y) syscall(322,x,y)
#define DECR 0
#define GET 1
#define SET 2

#include <stdio.h>
void block()
{
    pbarrier(DECR, 0);
    while(pbarrier(GET, 0));
//    printf("done");
}

#endif
