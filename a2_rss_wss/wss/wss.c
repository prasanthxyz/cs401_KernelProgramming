#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include "../arch/x86/include/asm/pgtable.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");


/* Linked list operations */
struct output_node
{
	unsigned long wss_count;
	struct output_node *next;
};
struct output_node *op_head = NULL;
struct output_node *op_tail = NULL;

static void insOPNode(unsigned long wssc)
{
	struct output_node *cur;
	cur = kmalloc(sizeof(struct output_node), GFP_KERNEL);
	cur->wss_count = wssc;
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
	int i = 0;
	printk(KERN_INFO "Time    WSS\n");
	while(tmp != NULL) {
		printk(KERN_INFO "%d    %lu\n", i, tmp->wss_count);
		i += 10;
		tmp = tmp->next;
	}
}


struct vma_node
{
	unsigned long page_addr;
	struct vma_node *next;
};
struct vma_node *head = NULL;

static void insNode(unsigned long pa)
{
	struct vma_node *cur;
	cur = kmalloc(sizeof(struct vma_node), GFP_KERNEL);
	cur->page_addr = pa;
	cur->next = head;
	head = cur;
}

static int srchNode(unsigned long pa)
{
	struct vma_node *tmp;
	tmp = head;
	while(tmp != NULL) {
		if( (pa >= tmp->page_addr) && (pa < tmp->page_addr+PAGE_SIZE) )
			return 1;
		tmp = tmp->next;
	}
	return 0;
}

static void clearNodes(void)
{
	struct vma_node *tmp;
	while(head != NULL)
	{
		tmp = head;
		head = head->next;
		kfree(tmp);
	}
}
/* linked list operations over */


extern void (*lkm_fault_handler)(struct task_struct *, unsigned long);

static int pid = -1;
module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "Process ID");

static int times = 0;
module_param(times, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(times, "Timer times");

static int delay = 10;

/*
 * Page Table Walk Function.
 * Returns the pointer to pte.
 */
static pte_t *get_pte(struct mm_struct *mm, unsigned long page_addr)
{
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
	return pte;
}

static pte_t *get_present_pte(struct mm_struct *mm, unsigned long page_addr)
{
	pte_t *pte;
	pte = get_pte(mm, page_addr);
	if( pte && (pte_val(*pte) & _PAGE_PRESENT) )
		return pte;
	return NULL;
}

unsigned long wss = 0;
struct task_struct *ts;

static void my_fault_handler(struct task_struct *tsk, unsigned long page_addr)
{
	pte_t *pte;
	if(tsk->pid != pid)
		return;

	pte = get_pte(tsk->mm, page_addr);

	if(pte == NULL)
		return;

	if( (pte_val(*pte) & _PAGE_PRESENT) || !(pte_val(*pte) & _PAGE_PROTNONE) )
		return;

	if(srchNode(page_addr)) {
		*pte = pte_set_flags(*pte, _PAGE_PRESENT);
		*pte = pte_clear_flags(*pte, _PAGE_PROTNONE);
		wss++;
	}
}

static void fault_all_pages(void)
{
	struct vm_area_struct *vma;
	unsigned long page_addr;
	pte_t *pte;

	ts = pid_task(find_vpid(pid), PIDTYPE_PID);
	if(ts == NULL) {
		printk(KERN_INFO "Process not found.\n");
		return;
	}

	vma = ts->mm->mmap;
	while(vma != NULL) {
		for(page_addr = vma->vm_start; page_addr < vma->vm_end; page_addr += PAGE_SIZE) {
			pte = get_present_pte(ts->mm, page_addr);
			if(pte == NULL)
				continue;
			*pte = pte_set_flags(*pte, _PAGE_PROTNONE);
			*pte = pte_clear_flags(*pte, _PAGE_PRESENT);
			insNode(page_addr);
		}
		vma = vma->vm_next;
	}
}

static void restore_all_pages(void)
{
	struct vma_node *temp;
	pte_t *pte;
	temp = head;
	while(temp != NULL) {
		ts = pid_task(find_vpid(pid), PIDTYPE_PID);
		if(!ts)
			return;
		pte = get_pte(ts->mm, temp->page_addr);
		if(pte != NULL && !(pte_val(*pte) & _PAGE_PRESENT) && (pte_val(*pte) & _PAGE_PROTNONE)) {
			*pte = pte_clear_flags(*pte, _PAGE_PROTNONE);
			*pte = pte_set_flags(*pte, _PAGE_PRESENT);
		}
		temp = temp->next;
	}
}


/*TIMER*/
static struct timer_list my_timer;

static void timed_operations(unsigned long data)
{
	printk(KERN_INFO "Wss = %lu\n", wss);
	insOPNode(wss);
	restore_all_pages();
	clearNodes();
	fault_all_pages();
	wss = 0;

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
	printk(KERN_INFO "Module started\n");

	fault_all_pages();
	lkm_fault_handler = my_fault_handler;

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
	del_timer_sync(&my_timer);
	lkm_fault_handler = NULL;
	restore_all_pages();
	clearNodes();

	printk(KERN_INFO "Module exiting\n");
	printOPNodes();
	clearOPNodes();
}

module_init(myinit);
module_exit(myexit);

