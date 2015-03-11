#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <linux/rbtree.h>
#include <linux/mm.h>

#include "print_term.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

static int pid = -1;
module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "Process ID");

static int __init myinit(void)
{
    struct task_struct *ts;
	struct vm_area_struct *vma = NULL;
	static char buffer[512];

	struct rb_root *root;
	struct rb_node *rbnode;

	if(pid == -1) {
        print_term("pid not given");
		return -1;
    }

	ts = pid_task(find_vpid(pid), PIDTYPE_PID);
	vma = ts->mm->mmap;

	root = &ts->mm->mm_rb;
	rbnode = rb_first(root);

	print_term("Output from ex4 for comparison: ");
	while(vma != NULL)
	{
		unsigned long start_ll, end_ll;
		unsigned long start_rb, end_rb;

		start_ll = vma->vm_start;
		end_ll = vma->vm_end;

		start_rb = rb_entry(rbnode, struct vm_area_struct, vm_rb)->vm_start;
		end_rb = rb_entry(rbnode, struct vm_area_struct, vm_rb)->vm_end;

		snprintf(buffer, sizeof(buffer), "%08lx-%08lx    %08lx-%08lx", start_ll, end_ll, start_rb, end_rb);
		print_term(buffer);
		if(!((start_ll == start_rb) && (end_ll == end_rb))) {
			print_term("failed");
			return -1;
		}

		rbnode = rb_next(rbnode);
		vma = vma-> vm_next;
	}
	print_term("success");
	return 0;
}

static void __exit myexit(void)
{
}

module_init(myinit);
module_exit(myexit);
