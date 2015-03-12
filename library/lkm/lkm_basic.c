#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

static int __init myinit(void)
{
    return 0;
}

static void __exit myexit(void)
{
}

module_init(myinit);
module_exit(myexit);
