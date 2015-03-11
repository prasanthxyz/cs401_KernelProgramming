#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kfifo.h>

#include <linux/fs.h>

#include <linux/slab.h>
#include <linux/mm.h>
#include <asm/io.h>

#ifndef VM_RESERVED
#define VM_RESERVED (VM_DONTEXPAND | VM_DONTDUMP)
#endif

MODULE_AUTHOR("Prasanth P");
MODULE_LICENSE("GPL");


#define DEVICENAME "my_char_device"
#define NPAGES 4

int major_number;
static int references = 0;
char *buffer;

/* mmap functions */
int device_mmap(struct file *fp, struct vm_area_struct *vma)
{
    struct page *page;
    buffer = (char *)kmalloc(NPAGES*PAGE_SIZE, GFP_KERNEL);
    page = virt_to_page(buffer);
    memcpy((char *)buffer, "KERNEL MSG", 11);
    remap_pfn_range(vma, vma->vm_start, page_to_pfn(page), vma->vm_end - vma->vm_start, vma->vm_page_prot);
    vma->vm_flags |= VM_RESERVED;
    return 0;
}

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
    kfree(buffer);
    fp->private_data = NULL;
    printk(KERN_INFO "%s: Device closed.\n", DEVICENAME);
    return 0;
}

static struct file_operations fops = {
    .open = device_open,
    .release = device_close,
    .mmap = device_mmap,
};

static int __init chardev_init(void)
{
    printk(KERN_ALERT "%s: Driver module started.\n", DEVICENAME);

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
    unregister_chrdev(major_number, DEVICENAME);
    printk(KERN_INFO "%s: Removed device from kernel.\n", DEVICENAME);
    printk(KERN_ALERT "%s: Driver module exiting.\n", DEVICENAME);
}

module_init(chardev_init);
module_exit(chardev_exit);
