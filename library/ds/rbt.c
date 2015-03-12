#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/rbtree.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

static int __init myinit(void)
{
	struct task_struct *ts;
	struct vm_area_struct *vma = NULL;

	struct rb_root *root;
	struct rb_node *rbnode;

	ts = pid_task(find_vpid(1), PIDTYPE_PID);
	vma = ts->mm->mmap;
    root = &ts->mm->mm_rb;
	rbnode = rb_first(root);

	while(vma != NULL)
	{
		unsigned long start_rb, end_rb;

		start_rb = rb_entry(rbnode, struct vm_area_struct, vm_rb)->vm_start;
		end_rb = rb_entry(rbnode, struct vm_area_struct, vm_rb)->vm_end;

        /* Process start_rb, end_rb */

		rbnode = rb_next(rbnode);
		vma = vma->vm_next;
	}
	return 0;
}

static void __exit myexit(void)
{
}

module_init(myinit);
module_exit(myexit);
