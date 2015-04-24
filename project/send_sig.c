#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

static int pid = -1;
module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "PID");

static int ch = -1;
module_param(ch, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "CHOICE");

static int __init myinit(void)
{
    struct task_struct *ts;
    if (pid == -1 || ch == -1)
        return 0;
    ts = pid_task(find_vpid(pid), PIDTYPE_PID);
    if(!ts)
        return 0;
    printk("%s\n", ts->comm);
    if(!ch)
        send_sig(SIGSTOP, ts, 0);
    else
        send_sig(SIGCONT, ts, 0);
    return 0;
}

static void __exit myexit(void)
{
}

module_init(myinit);
module_exit(myexit);
