#include <unistd.h>

int main()
{
	syscall(322, 10);
    // 322 was the system-call-id added to kernel code
	return 0;
}
