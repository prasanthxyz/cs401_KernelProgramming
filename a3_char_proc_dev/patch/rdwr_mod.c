#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<asm/uaccess.h>

#include <linux/mutex.h>
#include <linux/slab.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "circular_queue.h"
#include "qstate.h"


MODULE_AUTHOR("Prasanth P");
MODULE_LICENSE("GPL");

static DEFINE_MUTEX(mutex_dev);
static DEFINE_MUTEX(mutex_prodcons);

static struct cqueue cq;
static struct q_state qs;

struct cdev *mcdev;	/*this is the name of my char driver that i will be registering*/
int major_number;	/* will store the major number extracted by dev_t*/
dev_t dev_num;		/*will hold the major number that the kernel gives*/

int ret;

#define DEVICENAME "my_char_device"

static int device_open(struct inode *inode, struct file *fp)
{
	if(!mutex_trylock(&mutex_dev)) {
		printk(KERN_ALERT "%s: Device under use, unable to unlock.\n", DEVICENAME);
		return -1;
	}
	printk(KERN_INFO "%s: Device opened.\n", DEVICENAME);
	return 0;
}

static int device_close(struct inode *inode, struct file *fp)
{
	mutex_unlock(&mutex_dev);
	//printk(KERN_INFO "%s: Queue State\n", DEVICENAME);
	//print_QState(qs);
	printk(KERN_INFO "%s: Device closed.\n", DEVICENAME);
	return ret;
}

static ssize_t reader(struct file *fp, char *buf, size_t len, loff_t *ppos)
{
	int bytes_read, bytes_to_read, i;
	char *temp;

	if(!mutex_trylock(&mutex_prodcons)) {
		printk(KERN_ALERT "%s: Device under use, unable to read.\n", DEVICENAME);
		return -1;
	}

	bytes_to_read = (len > cq.counter)? cq.counter : len;
	temp = kmalloc(bytes_to_read*sizeof(char), GFP_KERNEL);
	for(i = 0; i < bytes_to_read; i++) {
		temp[i] = getCQ(&cq);
		read_QState(&qs, cq.front, cq.rear);
	}

	bytes_read = bytes_to_read - copy_to_user(buf, temp, bytes_to_read);
	printk(KERN_INFO "%s: Data read [%d bytes].\n", DEVICENAME, bytes_read);

	kfree(temp);
	mutex_unlock(&mutex_prodcons);
	return bytes_read;
}

static ssize_t writer(struct file *fp, const char *buf, size_t len, loff_t *ppos)
{
	int bytes_to_write, bytes_written = 0, i;

	if(!mutex_trylock(&mutex_prodcons)) {
		printk(KERN_ALERT "%s: Device under use, unable to write.\n", DEVICENAME);
		return -1;
	}

	bytes_to_write = (len > (QUEUE_SIZE-cq.counter))? (QUEUE_SIZE-cq.counter) : len;
	if(bytes_to_write == 0) {
		printk(KERN_INFO "%s: Memory full.\n", DEVICENAME);
		mutex_unlock(&mutex_prodcons);
		return -1;
	}

	for(i = 0; i < bytes_to_write; i++) {
		putCQ(buf[i], &cq);
		write_QState(&qs, cq.rear);
		bytes_written++;
	}
	printk(KERN_INFO "%s: Data written [%d bytes].\n", DEVICENAME, bytes_written);

	mutex_unlock(&mutex_prodcons);
	return bytes_written;
}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.write = writer,
	.read = reader,
	.release = device_close,
};



static int show_queue_stats(struct seq_file *m, void *v)
{
	char buf[512];
	get_QState(qs, buf); 
	seq_printf(m, buf);
	return 0;
}

static int proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, show_queue_stats, NULL);
}

struct file_operations proc_fops = {
	.owner = THIS_MODULE,
	.open = proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

int chardev_init(void)
{
	initCQ(&cq);
	initQState(&qs, QUEUE_SIZE);


	proc_create(DEVICENAME, 0, NULL, &proc_fops);

	printk(KERN_ALERT "%s: Driver module started.\n", DEVICENAME);

	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICENAME);
	if(ret < 0) {
		printk(KERN_ALERT "%s: Major number allocation failed.\n", DEVICENAME);
		return ret;
	}
	major_number = MAJOR(dev_num);
	printk(KERN_INFO "%s: Major number <%d> allocated.\n", DEVICENAME, major_number);

	mcdev = cdev_alloc();
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;

	ret = cdev_add(mcdev, dev_num, 1);
	if(ret < 0) {
		printk(KERN_ALERT "%s: Failure: Could not add device to kernel.\n", DEVICENAME);
		return ret;
	}
	else
		printk(KERN_INFO "%s: Device added to kernel.\n", DEVICENAME);
	return 0;
}

void chardev_exit(void)
{
	cdev_del(mcdev);

	remove_proc_entry(DEVICENAME, NULL);

	printk(KERN_INFO "%s: Removed device from kernel.\n", DEVICENAME);

	unregister_chrdev_region(dev_num, 1);
	printk(KERN_INFO "%s: Unregistered device numbers.\n", DEVICENAME);

	printk(KERN_ALERT "%s: Driver module exiting.\n", DEVICENAME);
}

module_init(chardev_init);
module_exit(chardev_exit);
