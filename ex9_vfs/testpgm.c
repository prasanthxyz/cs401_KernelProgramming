#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    char buf[256];
    int fd = open("testfile.txt", O_WRONLY|O_CREAT, 0640);
    scanf("%s", buf);
    printf("%s\n", buf);
    return 0;
}
