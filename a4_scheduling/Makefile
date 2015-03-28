obj-m += assignment.o

all: process_creator
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

process_creator: process_creator.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	-rm process_creator
