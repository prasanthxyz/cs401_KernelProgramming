#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>

#include <linux/cpumask.h>

#include "print_term.h"
char buffer[255];


/* for timer */
static int times = 10;
static int delay = 2;
static struct timer_list my_timer;
/* timer over */


/* for number of context switches and migrations */
static unsigned long long kStart, kEnd;
static unsigned long long context_switches = 0;
extern void (*hook_log_context_switch)(void);
extern unsigned long long nr_context_switches(void);
static void logcs(void)
{
    context_switches++;
}

static unsigned long long migrations = 0;
extern void (*hook_migration)(void);
static void logmigrations(void)
{
    migrations++;
}

/* linked list for storing */
struct data_item
{
    unsigned long long kCS;
    unsigned long long modCS;
    unsigned long long migrations;
    struct data_item *next;
};
struct data_item *head = NULL;
struct data_item *last = NULL;
void insert_data_item(unsigned long long kcs, unsigned long long modcs, unsigned long long migs)
{
	struct data_item *temp = NULL;

    temp = kmalloc(sizeof(struct data_item), GFP_KERNEL);
    temp->kCS = kcs;
    temp->modCS = modcs;
    temp->migrations = migs;
    temp->next = NULL;

	if(head == NULL) // empty list - allocate, point head, set hit-count 1
	{
        head = temp;
        last = temp;
		return;
	}
    last->next = temp;
    last = temp;
}
/* linked list over */
/* context switches and migrations over */


/* for run queue length calculation */
extern unsigned long run_q_len(unsigned long);
static int ncpu = 0;
char smallbuf[4];



static void timed_operations(unsigned long data)
{
    unsigned long i;
    static int time = 0;

    /* context switches and migrations */
    kEnd = nr_context_switches();
	hook_log_context_switch = NULL;
	hook_migration = NULL;
    insert_data_item(kEnd-kStart, context_switches, migrations);
    hook_log_context_switch = logcs;
    hook_migration = logmigrations;


    /* run queue length */
    sprintf(buffer, "%d\t\t", time);
    time += 2;
    for_each_online_cpu(i) {
        sprintf(smallbuf, "%lu\t", run_q_len(i));
        strcat(buffer, smallbuf);
    }
    printk(KERN_INFO "%s\n", buffer);

    if(times <= 0) {
        printk("Data collected. Please remove module\n");
		return;
	}
    times--;
	mod_timer(&my_timer, jiffies + delay*HZ);
}



static int __init initModule(void)
{
    int i;

	times--;
	init_timer(&my_timer);
	my_timer.function = timed_operations;
	my_timer.data = 0;
	my_timer.expires = jiffies + delay*HZ;
	add_timer(&my_timer);

	hook_log_context_switch = logcs;
    hook_migration = logmigrations;
    kStart = nr_context_switches();
    printk(KERN_INFO "Run-Queue lengths\n");
    for_each_online_cpu(i) {
        ncpu++;
    }
    sprintf(buffer, "Time\t");
    for(i = 0; i < ncpu; i++) {
        sprintf(smallbuf, "CPU%d\t", i);
        strcat(buffer, smallbuf);
    }
    printk(KERN_INFO "%s\n", buffer);

	print_term("Logging started.");

	return 0;
}

static void __exit exitModule(void)
{
    unsigned long long prevk = 0, prevm = 0, prevmigs = 0;
    int time = 0;
    struct data_item *cur = NULL;
    del_timer_sync(&my_timer);
    hook_log_context_switch = NULL;
    hook_migration = NULL;

    print_term("Context switches per every 2 seconds:");
    print_term("Time\tKernel\tModule");
    cur = head;
    while(cur != NULL) {
        sprintf(buffer, "%d\t%llu\t%llu", time, cur->kCS - prevk, cur->modCS - prevm);
        print_term(buffer);
        time += 2;
        prevk = cur->kCS;
        prevm = cur->modCS;
        cur = cur->next;
    }


    print_term("CPU migrations per every 2 seconds:");
    print_term("Time\tMigrations");
    time = 0;
    while(head != NULL) {
        cur = head;
        head = head->next;
        sprintf(buffer, "%d\t%llu", time, cur->migrations - prevmigs);
        print_term(buffer);
        time += 2;
        prevmigs = cur->migrations;
        kfree(cur);
	}
	print_term("Logging stopped.");
}

module_init(initModule);
module_exit(exitModule);

