#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

static int param = -1;
module_param(param, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(param, "Param description");

static int __init myinit(void)
{
    return 0;
}

static void __exit myexit(void)
{
}

module_init(myinit);
module_exit(myexit);
