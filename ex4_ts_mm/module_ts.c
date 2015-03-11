#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/pid.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

static int pid = -1;

module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "Process ID");

static int __init myinit(void)
{
	struct task_struct *ts = NULL;

    ts = pid_task(find_vpid(pid), PIDTYPE_PID);
    if(ts == NULL) {
        printk(KERN_INFO "Error\n");
        return -1;
    }

    printk(KERN_INFO "\n\nDETAILS OF THE PROCESS - 5 TASK_STRUCT MEMBERS\n");
    printk(KERN_INFO "The variables used are comm, pid, state, last_wakee and exit_state\n");
    printk(KERN_INFO "1. Process name      : %s\n", ts->comm);
    printk(KERN_INFO "2. Process id        : %d\n", ts->pid);
    switch(ts->state) {
        case -1:
            printk(KERN_INFO "3. Process state     : unrunnable\n");
            break;
        case 0:
            printk(KERN_INFO "3. Process state     : runnable\n");
            break;
        default:
            printk(KERN_INFO "3. Process state     : stopped\n");
            break;
    }
    printk(KERN_INFO "4. Last wakee        : %s (PID: %d)\n",
            ts->last_wakee->comm, ts->last_wakee->pid);
    printk(KERN_INFO "5. Exit state        : %d\n", ts->exit_state);
    printk(KERN_INFO "\nPROCESS HIERARCHY\n");
    do {
        ts = ts->parent;
        printk(KERN_INFO "%s (PID: %d)\n", ts->comm, ts->pid);
    } while(ts->pid != 1);
    return 0;
}

static void __exit myexit(void)
{
    printk(KERN_INFO "Module exited.\n");
}

module_init(myinit);
module_exit(myexit);

