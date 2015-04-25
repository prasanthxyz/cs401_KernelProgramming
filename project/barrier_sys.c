#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched.h>

#include <linux/spinlock.h>

MODULE_AUTHOR("Prasanth P");
MODULE_LICENSE("GPL");

#include "grptskds.c"

#define BARRIER_INIT 0
//arg1: gid; arg2: nproc
#define BARRIER_BLOCK 1
//arg1: gid

DEFINE_SPINLOCK(lock);

#include "sysfs.c"

extern long (*pbarrier)(struct task_struct *, int, int, int);

long pbarrierfn(struct task_struct *ts, int operation, int arg1, int arg2)
{
    int retval;
    spin_lock(&lock);

    //(arg1:gid, arg2:nproc)
    if(operation == BARRIER_INIT) {
        if(!createGroup(arg1, arg2)) {
            spin_unlock(&lock);
            return 1;
        }
    } else if (operation == BARRIER_BLOCK) {
        //(arg1: gid)
        retval = insertTask(arg1, ts);
        if(retval == 1) {
            freeGroup(arg1);
            spin_unlock(&lock);
            return 1;
        } else if(retval == 0) {
            send_sig(SIGSTOP, ts, 0);
        }
    }
    spin_unlock(&lock);
    return 0;
}

static int __init mod_sys_init(void)
{
    int retval;
    initSysFs();
    initialize_Lists();

    kobj = kobject_create_and_add("barrier", NULL);
    if (!kobj)
        return -ENOMEM;
    retval = sysfs_create_group(kobj, &attr_group);
    if (retval) {
        kobject_put(kobj);
        return -ENOMEM;
    }

    pbarrier = pbarrierfn;
    return 0;
}

static void __exit mod_sys_exit(void)
{
    pbarrier = NULL;
    kobject_put(kobj);
    freeLists();
}

module_init(mod_sys_init);
module_exit(mod_sys_exit);
