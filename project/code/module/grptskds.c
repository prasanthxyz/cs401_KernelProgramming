struct tsknode {
    struct task_struct *tsk;
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

/*
 * freeLists()
 * frees all the tasks in every group
 * frees all the groups
 */

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


/*
 * getGroup ( gid )
 *   traverses the groups linked list
 *   returns the group with given gid
 *   return values:
 *     node-address: SUCCESS
 *     NULL        : node-not-found
 */

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

/*
 * createGroup ( gid, nproc )
 *   creates a new group node
 *   initializes it with given gid and nproc
 *   return values:
 *     node_address: SUCCESS
 *     NULL        : group already present
 */

static struct groupnode *createGroup(int gid, int nproc)
{
    struct groupnode *tmp;

    if((gid <= 0) || (nproc <= 0) || getGroup(gid))
        return NULL;

    tmp = (struct groupnode *)kmalloc(sizeof(struct groupnode),GFP_KERNEL);
    tmp->gid = gid;
    tmp->nproc = nproc;
    INIT_LIST_HEAD(&(tmp->tsks.list));
    list_add_tail(&(tmp->list), &(groups.list));

    return tmp;
}

/*
 * freeGroup ( gid )
 *   clears the group with given gid
 *   sends SIGCONT to all the tasks in the group
 *   return values:
 *     0: SUCCESS
 *     1: GROUP_NOT_FOUND
 */

static int freeGroup(int gid)
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
                if(tmp2)
                    send_sig(SIGCONT, tmp2->tsk, 0);
                list_del(pos2);
                kfree(tmp2);
            }
            list_del(pos);
            kfree(tmp);
            return 0;
        }
    }
    return 1;
}

/*
 * printGroups(gid, buf)
 *   puts stats of group with given gid into buf
 *   if gid == -1, puts all groups' stats.
 *   (prints tasks in groups)
 */

static void printGroups(int gid, char *buf)
{
    struct groupnode *tmp;
    struct tsknode *tmptsk;

    int blocked;

    char temp[128], tidtmp[20], tmpbuf[256];
    strcpy(temp, "");
    strcpy(tidtmp, "");

    if(gid != -1) {
        tmp = getGroup(gid);
        if(!tmp) {
            strcpy(buf, "Group ID not found");
            return;
        }

        blocked = 0;
        list_for_each_entry(tmptsk, &tmp->tsks.list, list) {
            sprintf(tidtmp, "%d ", tmptsk->tsk->pid);
            strcat(temp, tidtmp);
            blocked++;
        }

        snprintf(buf, 1024, "\n\
                Group ID            : %d\n\
                Total no. of procs  : %d\n\
                No. of blocked procs: %d\n\
                PIDs                : %s\n",
                gid,
                blocked + tmp->nproc,
                blocked,
                temp);

    } else {
        strcpy(buf, "");
        list_for_each_entry(tmp, &groups.list, list) {
            strcpy(temp, "");
            strcpy(tidtmp, "");
            blocked = 0;
            list_for_each_entry(tmptsk, &tmp->tsks.list, list) {
                sprintf(tidtmp, "%d ", tmptsk->tsk->pid);
                strcat(temp, tidtmp);
                blocked++;
            }

            snprintf(tmpbuf, sizeof(tmpbuf),"\n\
                    Group ID            : %d\n\
                    Total no. of procs  : %d\n\
                    No. of blocked procs: %d\n\
                    PIDs                : %s\n",
                    tmp->gid,
                    blocked + tmp->nproc,
                    blocked,
                    temp);
            strcat(buf, tmpbuf);
        }
    }
}

/*
 * insertTask ( gid, tsk )
 *   inserts the task struct 'tsk' into the group-node with id 'gid'
 *   Return values
 *     0: SUCCESS             
 *     1: COUNTER_REACHED_ZERO
 *     2: GROUP_ID_NOT_FOUND  
 *     3: TASK_ALREADY_PRESENT
 *     4: INVALID_GID
 */

static int insertTask(int gid, struct task_struct *tsk)
{
    struct groupnode *tmpgrp;
    struct tsknode *tmptsk;

    if (gid <= 0)
        return 4;

    tmpgrp = getGroup(gid);
    if(!tmpgrp)
        return 2;

    list_for_each_entry(tmptsk, &(tmpgrp->tsks.list), list) {
        if(tmptsk->tsk->pid == tsk->pid) {
            return 3;
        }
    }

    tmptsk = (struct tsknode *)kmalloc(sizeof(struct tsknode),GFP_KERNEL);
    tmptsk->tsk = tsk;
    list_add_tail(&(tmptsk->list), &(tmpgrp->tsks.list));

    tmpgrp->nproc--;
    if(tmpgrp->nproc <= 0) {
        return 1;
    }
    return 0;
}
