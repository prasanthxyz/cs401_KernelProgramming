#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");


struct tsknode {
    int tid;
    //struct task_struct tsk;
    struct list_head list;
};

struct groupnode {
    int gid;
    struct tsknode tsks;
    struct list_head list;
}groups;

/*
struct node {
    char data;
    struct list_head list;
}start;
*/

static struct groupnode *insertGroup(int gid)
{
    struct groupnode *tmp;
    tmp = (struct groupnode *)kmalloc(sizeof(struct groupnode),GFP_KERNEL);
    INIT_LIST_HEAD(&(tmp->tsks.list));
    tmp->gid = gid;
    list_add_tail(&(tmp->list), &(groups.list));
    return tmp;
}

static void printGroups(void)
{
    struct groupnode *tmp;
    struct tsknode *tmptsk;
    list_for_each_entry(tmp, &groups.list, list) {
        printk("%d:", tmp->gid);
        list_for_each_entry(tmptsk, &tmp->tsks.list, list) {
            printk("%d ", tmptsk->tid);
        }
        printk("\n");
    }
}

static void insertTid(int gid, int tid)
{
    struct groupnode *tmpgrp;
    struct tsknode *tmptsk;
    int flag = 0;
    list_for_each_entry(tmpgrp, &groups.list, list) {
        if(tmpgrp->gid == gid) {
            flag = 1;
            break;
        }
    }
    if(!flag) {
        tmpgrp = insertGroup(gid);
    }

    tmptsk = (struct tsknode *)kmalloc(sizeof(struct groupnode),GFP_KERNEL);
    tmptsk->tid = tid;
    list_add_tail(&(tmptsk->list), &(tmpgrp->tsks.list));
}

/* _SYSFS_ start */
static int gid;
static int tid;
static int run;

static ssize_t gid_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf)
{
        return sprintf(buf, "%d\n", gid);
}
static ssize_t gid_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count)
{
        sscanf(buf, "%du", &gid);
        return count;
}

static ssize_t tid_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf)
{
        return sprintf(buf, "%d\n", tid);
}
static ssize_t tid_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count)
{
        sscanf(buf, "%du", &tid);
        return count;
}

static ssize_t run_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf)
{
        insertTid(gid, tid);
        printGroups();
        return sprintf(buf, "%d\n", run);
}
static ssize_t run_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count)
{
        sscanf(buf, "%du", &run);
        return count;
}
/* Sysfs attributes cannot be world-writable. */
static struct kobj_attribute gid_attribute =
        __ATTR(gid, 0664, gid_show, gid_store);
static struct kobj_attribute tid_attribute =
        __ATTR(tid, 0664, tid_show, tid_store);
static struct kobj_attribute run_attribute =
        __ATTR(run, 0664, run_show, run_store);

static struct attribute *attrs[] = {
        &gid_attribute.attr,
        &tid_attribute.attr,
        &run_attribute.attr,
        NULL,   /* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
        .attrs = attrs,
};

static struct kobject *kobj;
/* _SYSFS_ end */

static int __init myinit(void)
{
    int retval;

    kobj = kobject_create_and_add("ll", NULL);
    if (!kobj)
        return -ENOMEM;
    retval = sysfs_create_group(kobj, &attr_group);
    if (retval)
        kobject_put(kobj);

    //int i;
    INIT_LIST_HEAD(&groups.list);

    /* Insertion */
    /*
    tmp = (struct node *)kmalloc(sizeof(struct node),GFP_KERNEL);
    tmp->data = i;
    list_add_tail(&(tmp->list), &(start.list));
    // list_add() for adding at head
    */

    /* Traversal */
    //list_for_each_entry(tmp, &start.list, list) {
    //    /* process tmp->data */
    //}
	return 0;
}

static void __exit myexit(void)
{
    struct list_head *pos, *q;
    struct groupnode *tmp;

    kobject_put(kobj);

    list_for_each_safe(pos, q, &groups.list) {
        struct list_head *pos2, *q2;
        struct tsknode *tmp2;
        tmp = list_entry(pos, struct groupnode, list);

        list_for_each_safe(pos2, q2, &(tmp->tsks.list)) {
            tmp2 = list_entry(pos2, struct tsknode, list);
            list_del(pos2);
            kfree(tmp2);
        }

        list_del(pos);
        kfree(tmp);
    }
//
//    /* Freeing memory... */
//    list_for_each_safe(pos, q, &start.list) {
//        tmp = list_entry(pos, struct node, list);
//        list_del(pos);
//        kfree(tmp);
//    }
}

module_init(myinit);
module_exit(myexit);
