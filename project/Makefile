obj-m += barrier_sys.o

all: cprogram
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

cprogram: cprogram.c pbarrier.h

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	-rm cprogram
