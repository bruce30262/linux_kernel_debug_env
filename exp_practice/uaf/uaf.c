#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/cred.h>
#include "ioctl_header.h"

char *gb;

int simple_proc_open(struct inode * sp_inode, struct file *sp_file)
{
    printk(KERN_INFO "proc called open\n");
    return 0;
}
int simple_proc_release(struct inode *sp_indoe, struct file *sp_file)
{
    printk(KERN_INFO "proc called release\n");
    return 0;
}

ssize_t simple_proc_read(struct file *sp_file,char __user *buf, size_t size, loff_t *offset)
{
    printk(KERN_INFO "proc called read %d\n",size);
    copy_to_user(buf,gb,size);
    return size;
}
ssize_t simple_proc_write(struct file *sp_file,const char __user *buf, size_t size, loff_t *offset)
{
    printk(KERN_INFO "Current UID = %d\n",  current_uid());
    printk(KERN_INFO "proc called write %d\n",size);
    copy_from_user(gb,buf,size);
    printk(KERN_INFO "Current UID = %d\n",  current_uid());
    return size;
}

static long myioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch(cmd)
    {
        case UAF_IOCTL_MALLOC:
            printk("sizeof cred: %d\n", sizeof(struct cred));
            printk("kmalloc: %ld\n", arg);
            gb = kmalloc(arg, GFP_KERNEL);
            printk("malloc addr: %p\n", gb);
            break;
        case UAF_IOCTL_FREE:
            printk("kfree: %p", gb);
            kfree(gb);
            break;
        default:
            printk("Invalid command: %ld\n", cmd);
            break;
    }    
    return 0;
}

struct file_operations fops = {
    .open = simple_proc_open,
    .read = simple_proc_read,
    .write = simple_proc_write,
    .release = simple_proc_release,
    .unlocked_ioctl = myioctl
};

static int __init init_simpleproc (void)
{
    printk(KERN_INFO "init simple proc\n");
    if (! proc_create("simpleproc",0666,NULL,&fops)) {
        printk(KERN_INFO "ERROR! proc_create\n");
        remove_proc_entry("simpleproc",NULL);
        return -1;
    }
    return 0;
}
static void __exit exit_simpleproc(void)
{
    remove_proc_entry("simpleproc",NULL);
    printk(KERN_INFO "exit simple proc\n");
}

module_init(init_simpleproc);
module_exit(exit_simpleproc);
