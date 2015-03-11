obj-m += intercept.o

all: call_syscall
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

call_syscall: call_syscall.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm call_syscall
