obj-m += vfs2.o

all: testpgm2
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

testpgm: testpgm2.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	-rm testpgm2
