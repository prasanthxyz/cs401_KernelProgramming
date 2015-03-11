#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P <prasanth@cse.iitb.ac.in>");

static int __init myinit(void)
{
	printk(KERN_ALERT "Hello World\n");
	return 0;
}

static void __exit myexit(void)
{
	printk(KERN_ALERT "Bye World\n");
}

module_init(myinit);
module_exit(myexit);

