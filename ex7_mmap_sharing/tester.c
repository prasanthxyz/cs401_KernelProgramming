#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096

int main()
{
    char msg[100];
    int pn, ch, i;

    int configfd;
    configfd = open("/dev/my_char_device", O_RDWR);
    if(configfd < 0) {
        perror("open");
        return -1;
    }

    char *address = NULL;
    address = mmap(NULL, 10*PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, configfd, 0);
    if (address == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    for(i = 0; i < 10; i++)
        memcpy(address + i*PAGE_SIZE, "", 1);

    while (1) {
        printf("Enter choice:\n");
        printf("1. Write into memory\n");
        printf("2. Read from memory\n");
        printf("3. Exit\n");
        printf(" >> ");
        scanf(" %d", &ch);
        switch (ch) {
            case 1:
                printf("Enter the message to write: ");
                scanf( " %s", msg);
                printf("Enter the page to write: ");
                scanf(" %d", &pn);
                memcpy(address + pn*PAGE_SIZE, msg, strlen(msg));
                break;
            case 2:
                printf("Details of 10 pages\n");
                for(i = 0; i < 10; i++) {
                    printf("PG%d: %s\n", i, address + i*PAGE_SIZE); 
                }
                break;
            case 3:
                return 0;
            default:
                printf("Invalid option\n");
        }
    }
    close(configfd);
    return 0;
}

