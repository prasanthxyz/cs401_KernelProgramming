#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <asm/paravirt.h>

unsigned long **sys_call_table;

static unsigned long **aquire_sys_call_table(void)
{
	unsigned long int offset = PAGE_OFFSET;
	unsigned long **sct;

	while (offset < ULLONG_MAX) {
		sct = (unsigned long **)offset;

		if (sct[__NR_close] == (unsigned long *) sys_close) 
			return sct;

		offset += sizeof(void *);
	}
	return NULL;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

asmlinkage long (*original_psyscall)(int x);

int set_addr_rw(long unsigned int _addr)
{
	unsigned int level;
	pte_t *pte = lookup_address(_addr, &level);
	if(pte->pte &~ _PAGE_RW) pte->pte |= _PAGE_RW;
	return 0;
}

int set_addr_ro(long unsigned int _addr)
{
	unsigned int level;
	pte_t *pte = lookup_address(_addr, &level);
	pte->pte = pte->pte &~ _PAGE_RW;
	return 0;
}

asmlinkage long new_psyscall(int x)
{
	printk(KERN_ALERT "Intercepted! Value was %d\n", x);
	return 0;
}

static int __init myinit(void)
{
	if(!(sys_call_table = aquire_sys_call_table()))
		return -1;

	set_addr_rw((unsigned long)sys_call_table);

	original_psyscall = (void *)sys_call_table[__NR_psyscall];
	sys_call_table[__NR_psyscall] = (unsigned long *)new_psyscall;

	printk(KERN_ALERT "Interceptor loaded and waiting...\n");
	return 0;
}

static void __exit myexit(void)
{
	if(!sys_call_table)
		return;
	sys_call_table[__NR_psyscall] = (unsigned long *)original_psyscall;
	set_addr_ro((unsigned long)sys_call_table);
	printk(KERN_ALERT "Interceptor removed; all good.\n");
}

module_init(myinit);
module_exit(myexit);
