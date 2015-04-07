#ifndef _PBARRIER_H_
#define _PBARRIER_H_

#define DECR 0
#define GET 1
#define SET 2

long pbarrier(int operation, int arg)
{
    return syscall(322, operation, arg);
}

#endif
