obj-m += barrier_sys.o
#obj-m += send_sig.o
#obj-m += ll_ll.o

#all: cprogram setnproc tester
all: cprogram tester
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

cprogram: cprogram.c pbarrier.h

cprogram2: cprogram2.c

setnproc: setnproc.c pbarrier.h

tester: tester.c pbarrier.h

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	-rm cprogram
	-rm cprogram2
	-rm setnproc
	-rm tester
