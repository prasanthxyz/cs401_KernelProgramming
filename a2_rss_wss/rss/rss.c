#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include "../arch/x86/include/asm/pgtable.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

static int pid = -1;
module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "Process ID");

static int times = 0;
module_param(times, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(times, "Timer times");

static int delay = 10;

/* Linked list operations */
struct output_node
{
	long vm_count;
	long rss_count;
	struct output_node *next;
};
struct output_node *op_head = NULL;
struct output_node *op_tail = NULL;

static void insOPNode(long vmc, long rssc)
{
	struct output_node *cur;
	cur = kmalloc(sizeof(struct output_node), GFP_KERNEL);
	cur->vm_count = vmc;
	cur->rss_count = rssc;
	cur->next = NULL;

	if(op_head == NULL || op_tail == NULL) {
		op_head = op_tail = cur;
		return;
	}
	op_tail->next = cur;
	op_tail = cur;
}
static void clearOPNodes(void)
{
	struct output_node *tmp;
	while(op_head != NULL)
	{
		tmp = op_head;
		op_head = op_head->next;
		kfree(tmp);
	}
	op_tail = NULL;
}
static void printOPNodes(void)
{
	struct output_node *tmp = op_head;
	printk(KERN_INFO "VMSize    RSS_Size\n");
	while(tmp != NULL) {
		printk(KERN_INFO "%ld    %ld\n", tmp->vm_count, tmp->rss_count);
		tmp = tmp->next;
	}
}
/* Linked list operations over */


static pte_t *get_pte(struct mm_struct *mm, unsigned long page_addr) {
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	pgd = pgd_offset(mm, page_addr);
	if(pgd_none(*pgd) || pgd_bad(*pgd)) {
		return NULL;
	}

	pud = pud_offset(pgd, page_addr);
	if(pud_none(*pud) || pud_bad(*pud)) {
		return NULL;
	}

	pmd = pmd_offset(pud, page_addr);
	if(pmd_none(*pmd) || pmd_bad(*pmd)) {
		return NULL;
	}

	pte = pte_offset_kernel(pmd, page_addr);
	if(pte && pte_present(*pte)) {
		return pte;
	}
	return NULL;
}


/*TIMER*/
static struct timer_list my_timer;

static void timed_operations(unsigned long data)
{
	long vpages = 0, rss = 0;
	pte_t *pte;
	unsigned long page_addr;
	struct task_struct *ts = NULL;
	struct vm_area_struct *vma = NULL;

	ts = pid_task(find_vpid(pid), PIDTYPE_PID);
	if(ts == NULL) {
		printk(KERN_INFO "Error; process not found.\n");
		return;
	}
	vma = ts->mm->mmap;
	while(vma != NULL) {
		for(page_addr = vma->vm_start; page_addr < vma->vm_end; page_addr += PAGE_SIZE) {
			pte = get_pte(ts->mm, page_addr);
			vpages++;
			if(pte != NULL)
				rss++;
		}
		vma = vma->vm_next;
	}
	printk(KERN_INFO "Virtual Pages: %ld (%ld K)\n", vpages, vpages*PAGE_SIZE/1024);
	printk(KERN_INFO "RSS Pages    : %ld (%ld K)\n", rss, rss*PAGE_SIZE/1024);
	insOPNode(vpages*PAGE_SIZE/1024, rss*PAGE_SIZE/1024);

	if(times <= 0) {
		printk(KERN_INFO "Logging completed. Remove module to print log\n");
		return;
	}
	else
		times--;

	mod_timer(&my_timer, jiffies + delay*HZ);
}
/*TIMER finished*/


static int __init myinit(void)
{
	struct task_struct *ts = NULL;
	ts = pid_task(find_vpid(pid), PIDTYPE_PID);

	if(ts == NULL) {
		printk(KERN_INFO "Error; process not found.\n");
		return 0;
	}
	printk(KERN_INFO "Process name : %s\n", ts->comm);
	printk(KERN_INFO "Process id   : %d\n", ts->pid);
	printk(KERN_INFO "Page size    : %lu KB\n", PAGE_SIZE/1024);


	times--;
	init_timer(&my_timer);
	my_timer.function = timed_operations;
	my_timer.data = 0;
	my_timer.expires = jiffies + delay*HZ;
	add_timer(&my_timer);

	return 0;
}

static void __exit myexit(void)
{
	printk(KERN_INFO "Module exiting.\n");
	printOPNodes();
	clearOPNodes();
}

module_init(myinit);
module_exit(myexit);

