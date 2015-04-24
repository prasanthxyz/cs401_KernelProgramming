#obj-m += barrier_sys.o
obj-m += ll_ll.o

#all: cprogram setnproc tester
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

cprogram: cprogram.c pbarrier.h

setnproc: setnproc.c pbarrier.h

tester: tester.c pbarrier.h

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	-rm cprogram
	-rm setnproc
	-rm tester
