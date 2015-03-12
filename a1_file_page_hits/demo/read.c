#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	char x;
	int i = 0, temp;
	char fname[255];
	scanf("%s", fname);
	int f = open(fname, O_RDONLY, S_IWRITE | S_IREAD);
	for(;;)
	{
		temp = read(f, &x, 1);
		i++;
		if(temp == 0)
			break;
		//printf("%c ", x);
	}
	printf("\n read() was called %d times\n", i);
	close(f);
	return 0;
}
