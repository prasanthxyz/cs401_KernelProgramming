#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/slab.h>

#include "print_term.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

// linked list structure
struct tsnode_list {
	struct list_head list; // linked list
	pid_t pid;
	char comm[TASK_COMM_LEN];
	long state;
	struct task_struct *last_wakee;
	int exit_state;
};
struct tsnode_list tslist;

static int __init myinit(void)
{
	struct task_struct *ts = NULL, *tmpts = NULL;
	static char buffer[512];
	static char yes[] = "success";
	static char no[] = "failed";
	int even_count = 0;

	struct tsnode_list *tmp;   // for traversal
	struct list_head *pos, *q; // for deletion

	INIT_LIST_HEAD(&tslist.list);

	print_term("Module started.");

	print_term("Printing the process details from list in system.");
	snprintf(buffer, sizeof(buffer), "%-5s %-30s %-5s %-30s %-5s", "PID", "NAME", "STATE", "LAST_WAKEE", "EXIT_STATE");
	print_term(buffer);

	for_each_process(ts) {
		snprintf(buffer, sizeof(buffer), "%-5d %-30s %-5ld %-30s %-5d", ts->pid, ts->comm, ts->state, ts->last_wakee->comm, ts->exit_state);
		print_term(buffer);
	}

	print_term("Filling the linked list with process details.");
	for_each_process(ts) {
		tmp = (struct tsnode_list *)kmalloc(sizeof(struct tsnode_list),GFP_KERNEL);

		tmp->pid = ts->pid;
		strcpy(tmp->comm,ts->comm);
		tmp->state = ts->state;
		tmp->last_wakee = ts->last_wakee;
		tmp->exit_state = ts->exit_state;

		list_add_tail(&(tmp->list), &(tslist.list));
	}

	print_term("Printing the process details from my linked list, and checking it against kernel list.");
	snprintf(buffer, sizeof(buffer), "%-5s %-30s %-5s %-30s %-5s %s", "PID", "NAME", "STATE", "LAST_WAKEE", "EXIT", "SYSTEM_LIST_CHK_PID");
	print_term(buffer);
	tmpts = &init_task;
	tmpts = next_task(tmpts);
	list_for_each_entry(tmp, &tslist.list, list) {
		if(tmp->pid == tmpts->pid)
			snprintf(buffer, sizeof(buffer), "%-5d %-30s %-5ld %-30s %-5d %s", tmp->pid, tmp->comm, tmp->state, tmp->last_wakee->comm, tmp->exit_state, yes);
		else
			snprintf(buffer, sizeof(buffer), "%-5d %-30s %-5ld %-30s %-5d %s", tmp->pid, tmp->comm, tmp->state, tmp->last_wakee->comm, tmp->exit_state, no);
		print_term(buffer);
		tmpts = next_task(tmpts);
	}

	print_term("Printing head of lists:");
	tmpts = next_task(&init_task);
	snprintf(buffer, sizeof(buffer), "Head of kernel list: Process %s [PID: %d]", tmpts->comm, tmpts->pid);
	print_term(buffer);
	tmp = list_first_entry(&tslist.list, struct tsnode_list, list);
	snprintf(buffer, sizeof(buffer), "Head of my list: Process %s [PID: %d]", tmp->comm, tmp->pid);
	print_term(buffer);

	print_term("Deleting every alternate entry from the linked list...");
	list_for_each_safe(pos, q, &tslist.list) {
		if(even_count%2 == 0) {
			tmp = list_entry(pos, struct tsnode_list, list);
			list_del(pos);
			kfree(tmp);
		}
		even_count++;
	}

	print_term("Printing the resulting custom list elements:");
	snprintf(buffer, sizeof(buffer), "%-5s %-30s %-5s %-30s %-5s %s", "PID", "NAME", "STATE", "LAST_WAKEE", "EXIT", "SYSTEM_LIST_CHK_PID");
	print_term(buffer);
	list_for_each_entry(tmp, &tslist.list, list) {
		snprintf(buffer, sizeof(buffer), "%-5d %-30s %-5ld %-30s %-5d", tmp->pid, tmp->comm, tmp->state, tmp->last_wakee->comm, tmp->exit_state);
		print_term(buffer);
	}

	return 0;
}

static void __exit myexit(void)
{
	struct list_head *pos, *q;
	struct tsnode_list *tmp;

	print_term("Freeing memory...");
	list_for_each_safe(pos, q, &tslist.list) {
		tmp = list_entry(pos, struct tsnode_list, list);
		list_del(pos);
		kfree(tmp);
	}
	print_term("Module exited.");
}

module_init(myinit);
module_exit(myexit);

