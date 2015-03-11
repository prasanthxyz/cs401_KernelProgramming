#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init myinit(void)
{
	printk(KERN_ALERT "Hello World 2\n");
	return 0;
}

static void __exit myexit(void)
{
	printk(KERN_ALERT "Bye 2\n");
}

module_init(myinit);
module_exit(myexit);
