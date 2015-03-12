#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/kfifo.h>

// int => DATA TYPE
static DECLARE_KFIFO_PTR(fifo, int);

static int __init myinit(void)
{
    int val;
    if( kfifo_alloc(&fifo, 10*sizeof(char), GFP_KERNEL) != 0 ) {
        /* Allocation error */
        return 0;
    }
    kfifo_put(&fifo, 1234567);
    while(kfifo_get(&fifo, &val)) {
        /* use val */ 
    }
	return 0;
}

static void __exit myexit(void)
{
    kfifo_free(&fifo);
}

module_init(myinit);
module_exit(myexit);
