#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

MODULE_AUTHOR("Prasanth P");
MODULE_LICENSE("GPL");

extern long (*pbarrier)(int, int, int);

long pbarrierfn(int nproc, int operation, int arg)
{
    if(operation == 0) {
        return nproc-1;
    } else if (operation == 1) {
        return nproc;
    } else if (operation == 2) {
        return arg;
    } else {
        return -10;
    }
//    printk(KERN_INFO "%d\n", nproc);
    return nproc;
}

static int __init mod_sys_init(void)
{
    pbarrier = pbarrierfn;
    return 0;
}

static void __exit mod_sys_exit(void)
{
    pbarrier = NULL;
}

module_init(mod_sys_init);
module_exit(mod_sys_exit);
