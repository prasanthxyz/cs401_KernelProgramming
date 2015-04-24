struct tsknode {
    int tid;
    //struct task_struct tsk;
    struct list_head list;
};

struct groupnode {
    int gid;
    int nproc;
    struct tsknode tsks;
    struct list_head list;
}groups;

static void initialize_Lists(void)
{
    INIT_LIST_HEAD(&groups.list);
}

static void freeLists(void)
{
    struct list_head *pos, *q;
    struct groupnode *tmp;

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
}

static struct groupnode *getGroup(int gid)
{
    struct groupnode *tmp;

    list_for_each_entry(tmp, &groups.list, list) {
        if(tmp->gid == gid) {
            return tmp;
        }
    }
    return NULL;
}

static struct groupnode *createGroup(int gid, int nproc)
{
    struct groupnode *tmp;

    if(getGroup(gid))
        return NULL;

    tmp = (struct groupnode *)kmalloc(sizeof(struct groupnode),GFP_KERNEL);
    tmp->gid = gid;
    tmp->nproc = nproc;
    INIT_LIST_HEAD(&(tmp->tsks.list));
    list_add_tail(&(tmp->list), &(groups.list));

    return tmp;
}

static void freeGroup(int gid)
{
    struct list_head *pos, *q;
    struct groupnode *tmp;

    list_for_each_safe(pos, q, &groups.list) {
        tmp = list_entry(pos, struct groupnode, list);
        if(tmp->gid == gid) {
            struct list_head *pos2, *q2;
            struct tsknode *tmp2;

            list_for_each_safe(pos2, q2, &(tmp->tsks.list)) {
                tmp2 = list_entry(pos2, struct tsknode, list);
                list_del(pos2);
                kfree(tmp2);
            }
            list_del(pos);
            kfree(tmp);
            break;
        }
    }
}

static void printGroups(void)
{
    struct groupnode *tmp;
    struct tsknode *tmptsk;
    list_for_each_entry(tmp, &groups.list, list) {
        printk("%d(%d): ", tmp->gid, tmp->nproc);
        list_for_each_entry(tmptsk, &tmp->tsks.list, list) {
            printk(" %d", tmptsk->tid);
        }
        printk("\n");
    }
}

static void insertTask(int gid, int tid)
{
    struct groupnode *tmpgrp;
    struct tsknode *tmptsk;

    tmpgrp = getGroup(gid);

    if(!tmpgrp)
        return;

    list_for_each_entry(tmptsk, &(tmpgrp->tsks.list), list) {
        if(tmptsk->tid == tid) {
            return;
        }
    }

    tmptsk = (struct tsknode *)kmalloc(sizeof(struct tsknode),GFP_KERNEL);
    tmptsk->tid = tid;
    list_add_tail(&(tmptsk->list), &(tmpgrp->tsks.list));
}
