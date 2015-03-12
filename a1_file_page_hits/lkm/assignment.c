#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

struct file_node
{
	unsigned long inode;
	long hitc;
	struct file_node *next;
};

struct file_node *head = NULL;

/* Function: searchNode(inode, prev, cur)

	prev & cur NULL         - empty list
	prev NULL, cur NOT NULL - inode in head
	prev NOT NULL, cur NULL - inode not present
	prev & cur NOT NULL     - inode present in cur; prev is previous node
*/

void searchNode(unsigned long inode, struct file_node **prev, struct file_node **cur)
{
	*cur = NULL;
	*prev = NULL;

	if(head == NULL)
		return;

	*cur = head;
	while(*cur != NULL && (*cur)->inode != inode)
	{
		*prev = *cur;
		*cur = (*cur)->next;
	}
}

void rcdHit(unsigned long inode)
{
	struct file_node *cur, *prev;
	cur = NULL;
	prev = NULL;
	searchNode(inode, &prev, &cur);

	if(head == NULL) // empty list - allocate, point head, set hit-count 1
	{
		head = kmalloc(sizeof(struct file_node), GFP_KERNEL);

		head->inode = inode;
		head->hitc = 1;
		head->next = NULL;
		return;
	}

	if(prev != NULL && cur == NULL)		// the inode is head node
	{
		prev->next = kmalloc(sizeof(struct file_node), GFP_KERNEL);
		cur = prev->next;

		cur->inode = inode;
		cur->hitc = 1;
		cur->next = NULL;
		return;
	}

	cur->hitc++;
}

extern void (*record_file_hit)(unsigned long);

static int __init initModule(void)
{
	record_file_hit = rcdHit;
	printk(KERN_INFO "Page hit log started.\n");
	return 0;
}

static void __exit exitModule(void)
{
	struct file_node *cur;
	printk(KERN_INFO "INODE\tHITS\n");
	while(head != NULL)
	{
		cur = head;
		head = head->next;
		printk(KERN_INFO "%lu\t%ld\n", cur->inode, cur->hitc);
		kfree(cur);
	}
	record_file_hit = NULL;
	printk(KERN_INFO "Page hit log stopped.\n");
}

module_init(initModule);
module_exit(exitModule);
