#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<malloc.h>

#define DEVICE "/dev/my_char_device"
int fd = 0;

int write_device();
int read_device();

int main()
{
	int ch;
	char c;

	if(access(DEVICE, F_OK) == -1) {
		printf("%s: DRIVER_MODULE NOT LOADED\n.", DEVICE);
		return 0;
	}
	printf("%s: DRIVER_MODULE found.\n", DEVICE);

	while(1) {
		printf("char device CIRCULAR QUEUE\n");
		printf("1. Insert character\n");
		printf("2. Extract character\n");
		printf("3. Exit\n");
		scanf(" %d", &ch);

		switch(ch) {
		case 1:
			fd = open(DEVICE, O_RDWR);
			write_device();
			close(fd);
			break;
		case 2:
			fd = open(DEVICE, O_RDWR);
			read_device();
			close(fd);
			break;
		default:
			printf("Bye.\n");
			return 0;
		}

	}
	return 0;
}

int write_device()
{
	int write_length = 0, ret;

	char *data = (char *)malloc(1024 * sizeof(char));
	printf("Enter the characters to insert into queue: \n");
	scanf(" %[^\n]", data);
	write_length = strlen(data);

	ret = write(fd, data, write_length);
	if(ret < strlen(data))
		printf("QUEUE FULL.\n");
	if(ret >= 0)
		printf("Data inserted.\n");

	fflush(stdout);
	free(data);
	return 0;
}

int read_device()
{
	int read_length = 0, i;
	ssize_t ret;
	char *data = (char *)malloc(1024*sizeof(char));

	printf("Enter the number of characters to delete: ");
	scanf(" %d", &read_length);

	memset(data, 0, sizeof(data));
	ret = read(fd, data, read_length);
	if(ret < 0) {
		printf("Reading failed.\n");
	}
	else {
		for(i = 0; i < ret; i++)
			printf("%c ", data[i]);
		if(ret != read_length)
			printf("\nQUEUE EMPTY!");
		if(ret != 0)
			printf("\nData deleted.");
		printf("\n");
	}
	fflush(stdout);
	free(data);
	return 0;
}
