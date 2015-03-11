#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int hello3_data __initdata = 3;

static int __init myinit(void)
{
	printk(KERN_ALERT "Hello World %d\n", hello3_data);
	return 0;
}

static void __exit myexit(void)
{
	printk(KERN_ALERT "Bye 3\n");
}

module_init(myinit);
module_exit(myexit);
