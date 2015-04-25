#ifndef _PBARRIER_H_
#define _PBARRIER_H_

#define pbarrier(x,y,z) syscall(322,x,y,z)

#define BARRIER_INIT 0
//arg1: gid; arg2: nproc
#define BARRIER_BLOCK 1
//arg1: gid

#include <stdio.h>

void initGroup(int gid, int nproc)
{
    if (pbarrier(BARRIER_INIT, gid, nproc)) {
        printf("Group already exists.\n");
    }
}

void block(int gid)
{
    if(pbarrier(BARRIER_BLOCK, gid, 0)) {
        printf("barrier unlocked.\n");
    }
}

#endif
