#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h> 
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/kobject.h>
#include <linux/string.h>

#include <linux/sysfs.h>
#include <linux/ioctl.h>
#include <linux/rwsem.h>
#include "ioctl_defs.h"

static DECLARE_RWSEM(rwsem);

#ifndef VM_RESERVED
# define  VM_RESERVED   (VM_DONTEXPAND | VM_DONTDUMP)
#endif

MODULE_AUTHOR("Prasanth P");
MODULE_LICENSE("GPL");

#define DEVICENAME "my_char_device"
#define NPAGES 10
int major_number;
static unsigned long vir_pages[NPAGES];


/* sysfs */
static int sfs_share = 0;
static int sfs_npages = 0;

static ssize_t sfs_share_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", sfs_share);
}

static ssize_t sfs_share_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    sscanf(buf, "%du", &sfs_share);
    return count;
}

static struct kobj_attribute sfs_share_attribute =
	__ATTR(sfs_share, 0664, sfs_share_show, sfs_share_store);

static ssize_t sfs_npages_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", sfs_npages);
}

static ssize_t sfs_npages_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    sscanf(buf, "%du", &sfs_npages);
    return count;
}

static struct kobj_attribute sfs_npages_attribute =
	__ATTR(sfs_npages, 0664, sfs_npages_show, sfs_npages_store);

static struct attribute *attrs[] = {
	&sfs_share_attribute.attr,
	&sfs_npages_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *dev_kobj;
/* sysfs over */


/* mmap functions */
static int mmap_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
    struct page *page;
    pgoff_t pgoff = vmf->pgoff;

    if(vir_pages[pgoff] == 0) {
        vir_pages[pgoff] = get_zeroed_page(GFP_KERNEL);
    }
    /* get the page */
    page = virt_to_page(vir_pages[pgoff]);
    get_page(page);
    vmf->page = page;
    return 0;
}

struct vm_operations_struct mmap_vm_ops = {
	.fault = mmap_fault,
};

int device_mmap(struct file *fp, struct vm_area_struct *vma)
{
    int req_pages = (vma->vm_end - vma->vm_start)/PAGE_SIZE;

    if( (req_pages > sfs_npages) || (sfs_share != 1) )
        return -EACCES;

    vma->vm_ops = &mmap_vm_ops;
    vma->vm_flags |= VM_RESERVED;
    vma->vm_private_data = fp->private_data;
    return 0;
}



/* ioctl functions */

static long device_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case IOCTL_RDL:
            down_read(&rwsem);
            break;

        case IOCTL_RDUL:
            up_read(&rwsem);
            break;

        case IOCTL_WRL:
            down_write(&rwsem);
            break;

        case IOCTL_WRUL:
            up_write(&rwsem);
            break;

        default:
            return -EINVAL;
    }
    return 0;
}



/* device functions */
static int device_open(struct inode *inode, struct file *fp)
{
    try_module_get(THIS_MODULE);
    printk(KERN_INFO "%s: Device opened.\n", DEVICENAME);
    return 0;
}

static int device_close(struct inode *inode, struct file *fp)
{
    module_put(THIS_MODULE);
    printk(KERN_INFO "%s: Device closed.\n", DEVICENAME);
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_close,
    .mmap = device_mmap,
    .unlocked_ioctl = device_ioctl
};


static int __init chardev_init(void)
{
    int i, retval;

    /* sysfs */
    dev_kobj = kobject_create_and_add("my_char_device", NULL);
    if (!dev_kobj)
        return -ENOMEM;
    retval = sysfs_create_group(dev_kobj, &attr_group);
    if (retval)
        kobject_put(dev_kobj);
    /* sysfs over */

    printk(KERN_ALERT "%s: Driver module started.\n", DEVICENAME);
    for(i = 0; i < NPAGES; i++)
        vir_pages[i] = 0;

    major_number = register_chrdev(0, DEVICENAME, &fops);
    if(major_number < 0) {
        printk(KERN_ALERT "%s: Major number allocation failed.\n", DEVICENAME);
        return major_number;
    }
    printk(KERN_INFO "%s: Major number <%d> allocated.\n", DEVICENAME, major_number);
    printk(KERN_INFO "%s: Device added to kernel.\n", DEVICENAME);
    return 0;
}

static void __exit chardev_exit(void)
{
    int i;

    kobject_put(dev_kobj);

    for(i = 0; i < NPAGES; i++)
        free_page(vir_pages[i]);
    unregister_chrdev(major_number, DEVICENAME);
    printk(KERN_INFO "%s: Removed device from kernel.\n", DEVICENAME);
    printk(KERN_ALERT "%s: Driver module exiting.\n", DEVICENAME);
}

module_init(chardev_init);
module_exit(chardev_exit);
