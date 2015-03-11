obj-m += mmap_dyn_mod.o

all: tester
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

user_proc: tester.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm tester
