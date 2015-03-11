#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096

int main()
{
    int configfd;
    configfd = open("/dev/my_char_device", O_RDWR);
    if(configfd < 0) {
        perror("open");
        return -1;
    }

    char *address = NULL;
    address = mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, configfd, 0);
    if (address == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    printf("Page 1 details: %s\n", address);
    memcpy(address + PAGE_SIZE , "USER MSG", 9);
    printf("Wrote into page 2\n");
    printf("Page 2 details: %s\n", address+PAGE_SIZE);

    close(configfd);
    return 0;
}

