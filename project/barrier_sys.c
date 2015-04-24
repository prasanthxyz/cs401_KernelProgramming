#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched.h>

MODULE_AUTHOR("Prasanth P");
MODULE_LICENSE("GPL");

#include "grptskds.c"

#define BARRIER_INIT 0
//arg1: gid; arg2: nproc
#define BARRIER_BLOCK 1
//arg1: gid

extern long (*pbarrier)(struct task_struct *, int, int, int);

long pbarrierfn(struct task_struct *ts, int operation, int arg1, int arg2)
{
    printk("%d\n", ts->pid);
    if(operation == BARRIER_INIT) {
        //(gid, nproc)
        if(!createGroup(arg1, arg2))
            return 1;
    } else if (operation == BARRIER_BLOCK) {
        //(gid)
        if(insertTask(arg1, ts) == 1) {
            freeGroup(arg1);
            return 1;
        } else {
            send_sig(SIGSTOP, ts, 0);
        }
    }
    return 0;
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
