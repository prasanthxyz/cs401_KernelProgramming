obj-m += mmap_dyn_mod.o
obj-m += mmap_stat_mod.o

all: tester
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

tester: tester.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm tester
