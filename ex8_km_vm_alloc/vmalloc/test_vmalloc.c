#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include "print_term.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

static unsigned long va = -1;
module_param(va, ulong, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(va, "Virtual Address");

char buf[512];

static int __init myinit(void)
{
    if (va == -1) {
        print_term("Parameter va not provided.");
        return -1;
    }
    sprintf(buf, "Current process pid: %d", current->pid);
    print_term(buf);
    sprintf(buf, "Memory area content: %s", (char*)va);
    print_term(buf);
    return 0;
}

static void __exit myexit(void)
{
}

module_init(myinit);
module_exit(myexit);
