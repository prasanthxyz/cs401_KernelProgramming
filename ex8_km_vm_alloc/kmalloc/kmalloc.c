#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/slab.h>

#include "print_term.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

char *mem;
char buf[512];

static int __init myinit(void)
{
    unsigned long pa_pamacro, pa_pagewalk, va;

    mem = kmalloc(4096, GFP_KERNEL);
    va = (unsigned long)mem;
    pa_pamacro = __pa(mem);
    pa_pagewalk = slow_virt_to_phys(mem);

    sprintf(buf, "Current process pid: %d", current->pid);
    print_term(buf);
    sprintf(buf, "VA                 : %lu", va);
    print_term(buf);
    sprintf(buf, "PA (by __pa macro) : %lu", pa_pamacro);
    print_term(buf);
    sprintf(buf, "PA (by pagewalk)   : %lu", pa_pagewalk);
    print_term(buf);

    if (pa_pamacro == pa_pagewalk)
        print_term("Physical addresses obtained were SAME.");
    else
        print_term("Physical addresses obtained were DIFFERENT.");
    
    strcpy(buf, "HELLO KERNEL; KMALLOC");
    memcpy(mem, buf, strlen(buf)+1);
    print_term("Wrote \"HELLO KERNEL; KMALLOC\" to the memory location..");

    return 0;
}

static void __exit myexit(void)
{
    kfree(mem);
}

module_init(myinit);
module_exit(myexit);
