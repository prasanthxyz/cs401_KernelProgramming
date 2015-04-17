#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/fdtable.h>
#include <linux/dcache.h>
#include <linux/time.h>
#include <linux/rtc.h>

#include "print_term.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

static char write_buf[256];
static char time_buf[120];

static char finput[256];
static char foutput[4096];

int pid = -1;

char fname[256];

static char* get_time(long seconds)
{
	unsigned long localtime;
	struct tm tm;

	localtime = (unsigned long)(seconds - (sys_tz.tz_minuteswest*60));
	time_to_tm(localtime, 0, &tm);

	snprintf(time_buf, sizeof(time_buf),"%04ld-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	return time_buf;
}

static int disp_file_details(void)
{
	int fd_num = 0;

	struct task_struct *tsk;
	struct fdtable *tsk_fdtable;
	struct file* tsk_file;
	char *cwd;
	char buf[256];

	if (pid == -1)
	{
		print_term(KERN_INFO "Invalid PID");
		return 0;
	}

	tsk = pid_task(find_vpid(pid), PIDTYPE_PID);
    strcpy(foutput, "");

    sprintf(write_buf, "PID: %d\n", tsk->pid);
    strcat(foutput, write_buf);
	
	rcu_read_lock();

	tsk_fdtable = files_fdtable(tsk->files);

	while(tsk_fdtable->fd[fd_num] != NULL)
	{
		tsk_file = tsk_fdtable->fd[fd_num];
		cwd = d_path(&tsk_file->f_path, buf, 256*sizeof(char));
		if(strcmp(fname, tsk_file->f_path.dentry->d_iname) == 0) {
            snprintf(foutput, sizeof(foutput),
                    "Name: %s\n\
                    Inode: %lu\n\
                    Access Time: %s\n\
                    Modified Time: %s\n\
                    Size (Bytes): %lld\n\
                    Size (Blocks): %lu\n\
                    Bytes Consumed: %hu\n\
                    Inode ref count : %d\n\
                    File ref count : %ld\n\
                    Current Offset : %lld\n\
                    Address of File Open function: %p\n\
                    Address of File Close function: %p\n",
                    cwd,
                    tsk_file->f_inode->i_ino,
                    get_time(tsk_file->f_inode->i_atime.tv_sec),
                    get_time(tsk_file->f_inode->i_mtime.tv_sec),
                    tsk_file->f_inode->i_size,
                    tsk_file->f_inode->i_blocks,
                    tsk_file->f_inode->i_bytes,
                    tsk_file->f_inode->i_count.counter,
                    tsk_file->f_count.counter,
                    tsk_file->f_pos,
                    tsk_file->f_op->open,
                    tsk_file->f_op->release);
        }
        fd_num++;
	}

	rcu_read_unlock();
	return 0;
}

static ssize_t finput_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf)
{
    return scnprintf(buf, 256, "%s\n", finput);	
}
static ssize_t foutput_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf)
{
    disp_file_details();
    //sprintf(foutput, "%s;%d", fname, pid);
    return scnprintf(buf, sizeof(foutput), "%s\n", foutput);	
}
static ssize_t finput_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count)
{
    strcpy(finput, buf);
    sscanf(finput, "%s", fname);
    sscanf(finput+strlen(fname), "%d", &pid);
    return count;
}
static ssize_t foutput_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count)
{
    strcpy(foutput, buf);
    return count;
}

static struct kobj_attribute finput_attribute =
        __ATTR(finput, 0664, finput_show, finput_store);
static struct kobj_attribute foutput_attribute =
        __ATTR(foutput, 0664, foutput_show, foutput_store);

static struct attribute *attrs[] = {
    &finput_attribute.attr,
    &foutput_attribute.attr,
    NULL,
};

static struct attribute_group attr_group = {
        .attrs = attrs,
};

static struct kobject *kobj;

static int __init vfs1_init(void)
{
	int ret;

	kobj = kobject_create_and_add("vfs", kernel_kobj);
	
	if(!kobj)
		return -ENOMEM;
	
	ret = sysfs_create_group(kobj, &attr_group);
	
	if (ret)
		kobject_put(kobj);
	

	return 0;
}

static void __exit vfs1_exit(void)
{
    kobject_put(kobj);
}

module_init(vfs1_init);
module_exit(vfs1_exit);
