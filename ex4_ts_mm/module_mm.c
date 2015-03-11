#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

static int pid = -1;

module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "Process ID");

static int __init myinit(void)
{
	char *fname;
	char blank[] = " ";     // for printing when no file-name available
	struct task_struct *ts = NULL;
	struct vm_area_struct *vma = NULL;

	ts = pid_task(find_vpid(pid), PIDTYPE_PID);
    if(ts == NULL) {
        printk(KERN_INFO "Error\n");
        return -1;
    }

    printk(KERN_INFO "Process name : %s\n", ts->comm);
    printk(KERN_INFO "Process id   : %d\n", ts->pid);
    vma = ts->mm->mmap;
    while(vma != NULL) {
        struct file *file = vma->vm_file;
        vm_flags_t flags = vma->vm_flags;
        unsigned long ino = 0;
        unsigned long long pgoff = 0;
        unsigned long start, end;
        dev_t dev = 0;

        if (file) {
            struct inode *inode = file_inode(vma->vm_file);
            dev = inode->i_sb->s_dev;
            ino = inode->i_ino;
            pgoff = ((loff_t)vma->vm_pgoff) << PAGE_SHIFT;
        }

        start = vma->vm_start;
        if (stack_guard_page_start(vma, start))
            start += PAGE_SIZE;
        end = vma->vm_end;
        if (stack_guard_page_end(vma, end))
            end -= PAGE_SIZE;

        if(vma->vm_file != NULL)
            fname = vma->vm_file->f_path.dentry->d_iname;
        else
            fname = blank;

        printk(KERN_INFO "%08lx-%08lx %c%c%c%c %08llx %02x:%02x %lu %s\n",
                start,
                end,
                flags & VM_READ ? 'r' : '-',
                flags & VM_WRITE ? 'w' : '-',
                flags & VM_EXEC ? 'x' : '-',
                flags & VM_MAYSHARE ? 's' : 'p',
                pgoff,
                MAJOR(dev), MINOR(dev), ino,
                fname);
        vma = vma-> vm_next;
    }
    return 0;
}

static void __exit myexit(void)
{
    printk(KERN_INFO "Module exited.\n");
}

module_init(myinit);
module_exit(myexit);

