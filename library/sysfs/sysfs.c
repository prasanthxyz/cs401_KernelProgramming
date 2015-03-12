#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>

MODULE_AUTHOR("Prasanth P");
MODULE_LICENSE("GPL");

static int foo;
static int baz;
static int bar;

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf)
{
        return sprintf(buf, "%d\n", foo);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count)
{
        sscanf(buf, "%du", &foo);
        return count;
}
/* Sysfs attributes cannot be world-writable. */
static struct kobj_attribute foo_attribute =
        __ATTR(foo, 0664, foo_show, foo_store);


/*
 * More complex function where we determine which variable is being accessed by
 * looking at the attribute for the "baz" and "bar" files.
 */
static ssize_t b_show(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
        int var;

        if (strcmp(attr->attr.name, "baz") == 0)
                var = baz;
        else
                var = bar;
        return sprintf(buf, "%d\n", var);
}

static ssize_t b_store(struct kobject *kobj, struct kobj_attribute *attr,
                       const char *buf, size_t count)
{
        int var;

        sscanf(buf, "%du", &var);
        if (strcmp(attr->attr.name, "baz") == 0)
                baz = var;
        else
                bar = var;
        return count;
}

static struct kobj_attribute baz_attribute =
        __ATTR(baz, 0664, b_show, b_store);
static struct kobj_attribute bar_attribute =
        __ATTR(bar, 0664, b_show, b_store);


/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *attrs[] = {
        &foo_attribute.attr,
        &baz_attribute.attr,
        &bar_attribute.attr,
        NULL,   /* need to NULL terminate the list of attributes */
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
        .attrs = attrs,
};

static struct kobject *kobj;

static int __init mod_sys_init(void)
{
        int retval;

        kobj = kobject_create_and_add("kobject_example", NULL);
        if (!kobj)
                return -ENOMEM;

        /* Create the files associated with this kobject */
        retval = sysfs_create_group(kobj, &attr_group);
        if (retval)
                kobject_put(kobj);

        return retval;
}

static void __exit mod_sys_exit(void)
{
        kobject_put(kobj);
}

module_init(mod_sys_init);
module_exit(mod_sys_exit);
