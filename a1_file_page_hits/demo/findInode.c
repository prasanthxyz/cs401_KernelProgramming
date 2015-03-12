#include <sys/stat.h>
#include <stdio.h>

int main()
{
	struct stat var;
	char fname[255];
	int ret;
	scanf("%s", fname);
	ret = stat(fname, &var);
	printf("Inode number: %lu\n", var.st_ino);
	return 0;
}
