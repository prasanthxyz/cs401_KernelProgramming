/* _SYSFS_ start */
static int gid;
static int stats;

static char write_buf[1024];

static void initSysFs(void)
{
    gid = -1;
}

static void printStats(void)
{
    printGroups(gid, write_buf);
}

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

static ssize_t stats_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf)
{
        printStats();
        return sprintf(buf, "%s\n", write_buf);
}
static ssize_t stats_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count)
{
        sscanf(buf, "%du", &stats);
        return count;
}

/* Sysfs attributes cannot be world-writable. */
static struct kobj_attribute gid_attribute =
        __ATTR(gid, 0664, gid_show, gid_store);
static struct kobj_attribute stats_attribute =
        __ATTR(stats, 0664, stats_show, stats_store);

static struct attribute *attrs[] = {
        &gid_attribute.attr,
        &stats_attribute.attr,
        NULL,   /* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
        .attrs = attrs,
};

static struct kobject *kobj;
/* _SYSFS_ end */
