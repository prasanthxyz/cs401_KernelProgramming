#include <stdio.h>
#include <unistd.h>

int main()
{
    int i;
    printf("%d\n", getpid());
    while(1) {
        for(i = 0; i < 10000; i++) {
            printf("%d %d\n", i, getpid());
            usleep(100000);
        }
    }
    return 0;
}
