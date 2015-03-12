#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_AUTHOR("Prasanth P");
MODULE_LICENSE("GPL");

static int proc_display(struct seq_file *m, void *v)
{
	seq_printf(m, "Hello proc!\n");
	return 0;
}

static int proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_display, NULL);
}

static const struct file_operations proc_fops = {
	.owner = THIS_MODULE,
	.open = proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init proc_mod_init(void)
{
	proc_create("my_proc", 0, NULL, &proc_fops);
	return 0;
}

static void __exit proc_mod_exit(void)
{
	remove_proc_entry("my_proc", NULL);
}

module_init(proc_mod_init);
module_exit(proc_mod_exit);
