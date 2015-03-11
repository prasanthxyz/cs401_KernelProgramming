obj-m += module_ts.o
obj-m += module_mm.o

all: test
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

test: test.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm test
