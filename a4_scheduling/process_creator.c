#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

int main(int argc, char *argv[])
{
    int i, n, delay = 2, cpu;
    int pid;

    cpu_set_t my_set;

    if(argc == 1) {
        printf("Usage: ./process_creator <nproc> [<delay> [<affine-cpu>]]\n");
        return -1;
    }

    n = atoi(argv[1]);
    if(argc >= 3)
        delay = atoi(argv[2]);
    if(argc == 4)
        cpu = atoi(argv[3]);

    printf("%d, %d\n", n, delay);
    for(i = 0; i < n; i++)
    {
        pid = fork();
        if(pid != 0) {
            if(argc == 4) {
                CPU_ZERO(&my_set);
                CPU_SET(cpu, &my_set);
                sched_setaffinity(0, sizeof(cpu_set_t), &my_set);
            }
            printf("ch%d\n", i);
            while(1);
            exit(0);
        }
        sleep(delay);
    }
    return 0;
}
