#include <linux/module.h>
#include <linux/fs.h>

#include <linux/mutex.h>
#include <asm/uaccess.h>

MODULE_AUTHOR("Prasanth P");
MODULE_LICENSE("GPL");

static char dev_mem[80];
static int dev_mem_size = 79;
#define DEVICENAME "my_char_device"
static DEFINE_MUTEX(dev_lock);
static int major_number;


static int device_open(struct inode *inode, struct file *fp)
{
    if( mutex_trylock(&dev_lock) != 1 )
        return -1;
    try_module_get(THIS_MODULE);
    printk(KERN_INFO "%s: Device opened.\n", DEVICENAME);
    return 0;
}

static int device_close(struct inode *inode, struct file *fp)
{
    module_put(THIS_MODULE);
    printk(KERN_INFO "%s: Device closed.\n", DEVICENAME);
    mutex_unlock(&dev_lock);
    return 0;
}

static ssize_t reader(struct file *fp, char *buf, size_t len, loff_t *ppos)
{
    int bytes_read, bytes_to_read;

    bytes_to_read = (len > dev_mem_size)? dev_mem_size : len;

    if(bytes_to_read == dev_mem_size) {
        printk(KERN_INFO "%s: Memory empty.\n", DEVICENAME);
        return -1;
    }
    
    bytes_read = bytes_to_read - copy_to_user(buf, dev_mem, bytes_to_read);
    printk(KERN_INFO "%s: Data read [%d bytes].\n", DEVICENAME, bytes_read);
    return bytes_read;
}

static ssize_t writer(struct file *fp, const char *buf, size_t len, loff_t *ppos)
{
    int bytes_to_write, bytes_written = 0;

    bytes_to_write = (len > dev_mem_size)? dev_mem_size : len;

    bytes_written = bytes_to_write - copy_from_user(dev_mem, buf, bytes_to_write);
    printk(KERN_INFO "%s: Data written [%d bytes].\n", DEVICENAME, bytes_written);
    return bytes_written;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_close,
    .read = reader,
    .write = writer,
};

int chardev_init(void)
{
    printk(KERN_ALERT "%s: Driver module started.\n", DEVICENAME);
    major_number = register_chrdev(0, DEVICENAME, &fops);
    if(major_number < 0) {
        printk(KERN_ALERT "%s: Device registration failed.\n", DEVICENAME);
        return major_number;
    }
    printk(KERN_INFO "%s: Major number <%d> allocated.\n", DEVICENAME, major_number);
    printk(KERN_INFO "%s: Device added to kernel.\n", DEVICENAME);
    return 0;
}

void chardev_exit(void)
{
    unregister_chrdev(major_number, DEVICENAME);
    printk(KERN_INFO "%s: Removed device from kernel.\n", DEVICENAME);
    printk(KERN_ALERT "%s: Driver module exiting.\n", DEVICENAME);
}

module_init(chardev_init);
module_exit(chardev_exit);
