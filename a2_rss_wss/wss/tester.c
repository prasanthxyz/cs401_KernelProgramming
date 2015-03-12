#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PAGE_SIZE 4096

int main(int argc, char *argv[])
{
	int i,r,times;
	char **array;

	if(argc != 2) {
		printf("Usage: ./tester <times>\n");
		return 0;
	}

	printf("Tester; Process ID: %d\n", getpid());

	array = malloc(10*sizeof(char*));
	for(i = 0; i < 10; i++) {
		array[i] = malloc(PAGE_SIZE*sizeof(char));
	}

	srand(time(NULL));

	times = atoi(argv[1]);
	while(times > 0) {
		r = rand()%10;
		printf("Accessing %d pages\n", r);
		for(i = 0; i < r; i++) {
			array[i][0] = 'a';
		}
		times--;
		sleep(10);
	}

	for(i = 0; i < 10; i++) {
		free(array[i]);
	}
	free(array);

	return 0;
}
