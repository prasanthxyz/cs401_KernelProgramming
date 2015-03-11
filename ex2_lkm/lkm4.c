#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#define DRIVER_AUTHOR "Prasanth P <prasanth@cse.iitb.ac.in>"
#define DRIVER_DESC "HW"

static int __init myinit(void)
{
	printk(KERN_ALERT "Hello World 4\n");
	return 0;
}

static void __exit myexit(void)
{
	printk(KERN_ALERT "Bye 4\n");
}

module_init(myinit);
module_exit(myexit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
