#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    char buf[256];
    printf("PID: %d\n", getpid());
    int fd = open("testfile.txt", O_RDWR|O_CREAT, 0640);
    read(fd, buf, 5);
    printf("FILE CONTENT: %s\n", buf);
    scanf("%s", buf);
    read(fd, buf, 30);
    printf("FILE CONTENT: %s\n", buf);
    scanf("%s", buf);
    printf("%s\n", buf);
    return 0;
}
