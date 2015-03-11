#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

static short int myshort = 1;
static int myint = 420;
static long int mylong = 9999;
static char *mystring = "blah";
static int myIntArray[2] = { -1, -1 };
static int arr_argc = 0;

module_param(myshort, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(myshort, "A short integer");
module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(myint, "An integer");
module_param(mylong, long, S_IRUSR);
MODULE_PARM_DESC(mylong, "A long integer");
module_param(mystring, charp, 0000);
MODULE_PARM_DESC(myshort, "A character string");

module_param_array(myIntArray, int, &arr_argc, 0000);
MODULE_PARM_DESC(myIntArray, "An array of integers");

static int __init myinit(void)
{
	int i;
	printk(KERN_ALERT "Hello World 5\n");
	printk(KERN_ALERT "myshort is a short integer: %hd\n", myshort);
	printk(KERN_ALERT "myint is an integer: %d\n", myint);
	printk(KERN_ALERT "mylong is a long integer: %ld\n", mylong);
	printk(KERN_ALERT "mystring is a string: %s\n", mystring);
	for(i = 0; i < (sizeof myIntArray / sizeof (int)); i++)
		printk(KERN_ALERT "myIntArray[%d] = %d\n", i, myIntArray[i]);
	
	printk(KERN_ALERT "got %d arguments for myIntArray.\n", arr_argc);
	return 0;
}

static void __exit myexit(void)
{
	printk(KERN_ALERT "Bye 5\n");
}

module_init(myinit);
module_exit(myexit);
