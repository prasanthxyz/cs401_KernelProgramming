#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

static int times = 5;
static int delay = 2;

static struct timer_list my_timer;

static void timed_operations(unsigned long data)
{
    printk("%d\n", times);
    if(times <= 0) {
		return;
	}
    times--;
	mod_timer(&my_timer, jiffies + delay*HZ);
}

static int __init myinit(void)
{
	times--;
	init_timer(&my_timer);
	my_timer.function = timed_operations;
	my_timer.data = 0;
	my_timer.expires = jiffies + delay*HZ;
	add_timer(&my_timer);
	return 0;
}

static void __exit myexit(void)
{
	del_timer_sync(&my_timer);
}

module_init(myinit);
module_exit(myexit);

