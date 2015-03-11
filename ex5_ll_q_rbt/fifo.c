#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/slab.h>

#include "print_term.h"

#include <linux/kfifo.h>

static DECLARE_KFIFO_PTR(fifo, pid_t);

// return the number of processes
static int nProc(void)
{
	int cnt = 0;
	struct task_struct *ts;
	for_each_process(ts)
		cnt++;
	return cnt;
}

// function that inserts to queue
static void producer(void)
{
	struct task_struct *ts;
	if(kfifo_alloc(&fifo, nProc()*sizeof(pid_t), GFP_KERNEL))
        return;
	for_each_process(ts)
		kfifo_put(&fifo, ts->pid);
}

// function that deletes from queue
static void consumer(void)
{
	char buffer[512];
	int val;
	while(kfifo_get(&fifo, &val)) {
		snprintf(buffer, sizeof(buffer), "%d", val);
		print_term(buffer);
	}
}

static int __init myinit(void)
{
	char buffer[512];
	struct task_struct *ts;

	print_term("Producing... ");
	producer();
	print_term("Consuming... ");
	consumer();
	print_term("Printing all pid in the kernel list for comparison: ");
	for_each_process(ts) {
		snprintf(buffer, sizeof(buffer), "%d", ts->pid);
		print_term(buffer);
	}
	return 0;
}

static void __exit myexit(void)
{
	kfifo_free(&fifo);
}

module_init(myinit);
module_exit(myexit);
