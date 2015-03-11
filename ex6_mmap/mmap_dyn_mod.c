#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/mm.h>

#ifndef VM_RESERVED
# define  VM_RESERVED   (VM_DONTEXPAND | VM_DONTDUMP)
#endif

MODULE_AUTHOR("Prasanth P");
MODULE_LICENSE("GPL");

#define DEVICENAME "my_char_device"
#define NPAGES 4

int major_number;
static unsigned long vir_pages[NPAGES];
static int references = 0;

/* mmap functions */

static int mmap_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
    struct page *page;
    pgoff_t pgoff = vmf->pgoff;

    if(vir_pages[pgoff] == 0) {
        vir_pages[pgoff] = get_zeroed_page(GFP_KERNEL);
        memcpy((char *)vir_pages[pgoff], "KERNEL MSG", 10);
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
    vma->vm_ops = &mmap_vm_ops;
    vma->vm_flags |= VM_RESERVED;
    vma->vm_private_data = fp->private_data;
    return 0;
}


/* device functions */
static int device_open(struct inode *inode, struct file *fp)
{
    if(references != 0)
        return -1;
    references++;
    try_module_get(THIS_MODULE);
    printk(KERN_INFO "%s: Device opened.\n", DEVICENAME);
    return 0;
}

static int device_close(struct inode *inode, struct file *fp)
{
    references--;
    module_put(THIS_MODULE);
    printk(KERN_INFO "%s: Device closed.\n", DEVICENAME);
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_close,
    .mmap = device_mmap,
};

int chardev_init(void)
{
    int i;
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

void chardev_exit(void)
{
    int i;
    for(i = 0; i < NPAGES; i++)
        free_page(vir_pages[i]);
    unregister_chrdev(major_number, DEVICENAME);
    printk(KERN_INFO "%s: Removed device from kernel.\n", DEVICENAME);
    printk(KERN_ALERT "%s: Driver module exiting.\n", DEVICENAME);
}

module_init(chardev_init);
module_exit(chardev_exit);
