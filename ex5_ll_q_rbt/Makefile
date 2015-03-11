obj-m += ll.o
obj-m += fifo.o
obj-m += rbt.o

all: print_term.h
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
