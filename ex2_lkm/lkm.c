#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
	printk(KERN_ALERT "Hello World 1.\n");
	return 0;
}

void cleanup_module()
{
	printk(KERN_ALERT "Bye 1.\n");
}
