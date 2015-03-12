#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/list.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

struct node {
    char data;
    struct list_head list;
}start;

static int __init myinit(void)
{
    int i;

	struct node *tmp;
    INIT_LIST_HEAD(&start.list);

    /* Insertion */
    tmp = (struct node *)kmalloc(sizeof(struct node),GFP_KERNEL);
    tmp->data = i;
    list_add_tail(&(tmp->list), &(start.list));
    // list_add() for adding at head

    /* Traversal */
    list_for_each_entry(tmp, &start.list, list) {
        /* process tmp->data */
    }
	return 0;
}

static void __exit myexit(void)
{
    struct list_head *pos, *q;
    struct node *tmp;

    /* Freeing memory... */
    list_for_each_safe(pos, q, &start.list) {
        tmp = list_entry(pos, struct node, list);
        list_del(pos);
        kfree(tmp);
    }
}

module_init(myinit);
module_exit(myexit);

