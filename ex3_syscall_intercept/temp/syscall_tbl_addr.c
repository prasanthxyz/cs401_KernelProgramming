#include <stdio.h>

int main()
{
	FILE *map = fopen("/boot/System.map-3.18.1", "r");
	char line[256];
	fread(line, 256, 1, map);
	while(!feof(map))
	{
		fread(line, 256, 1, map);
	}
	unsigned long syscall_table = (unsigned long)atoi(system("cat /boot/System.map-3.18.1 | grep sys_call_table | cut -f1 -d ' ' | head -1"));
	printf("%lx\n", syscall_table);
	return 0;
}
