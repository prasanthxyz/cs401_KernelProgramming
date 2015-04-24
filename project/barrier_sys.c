#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched.h>

MODULE_AUTHOR("Prasanth P");
MODULE_LICENSE("GPL");

#include "grptskds.c"

extern long (*pbarrier)(int, int, int);

long pbarrierfn(int pid, int operation, int arg1, int arg2)
{
    printk("%du", current->pid);
    //insertTid(operation, arg);
    printGroups();
    return 0;
    /*
    if(operation != 1)
        printk("%d\t%d\n", operation, arg);
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
    */
}

static int __init mod_sys_init(void)
{
    initialize_Lists();
    pbarrier = pbarrierfn;
    return 0;
}

static void __exit mod_sys_exit(void)
{
    pbarrier = NULL;
    freeLists();
}

module_init(mod_sys_init);
module_exit(mod_sys_exit);
