#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

static short int choice = -1;

module_param(choice, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(choice, "A boolean choice variable");

static int __init myinit(void)
{
	printk(KERN_ALERT "Hello Param World\n");
	if(choice == 0)
		printk(KERN_ALERT "Choice FALSE\n");
	else if(choice == 1)
		printk(KERN_ALERT "Choice TRUE\n");
	else if(choice == -1)
		printk(KERN_ALERT "Choice at default value\n");
	return 0;
}

static void __exit myexit(void)
{
	printk(KERN_ALERT "Bye Param World\n");
}

module_init(myinit);
module_exit(myexit);
