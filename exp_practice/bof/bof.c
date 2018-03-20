#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
//#include <asm-generic/uaccess.h>

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
    char local[100] = "how are you";
    size_t len = size;
    char *p = local;
    while(len--)
    {
        copy_to_user(buf, p, 1);
        p++;
        buf++;
    }
    return size;   
}

ssize_t simple_proc_write(struct file *sp_file,const char __user *buf, size_t size, loff_t *offset)
{
    char local[100]={};
    size_t len = size;
    char *p = local;
    while(len--)
    {
        copy_from_user(p, buf, 1);
        p++;
        buf++;
    }
    return size;
}

struct file_operations fops = {
    .open = simple_proc_open,
    .read = simple_proc_read,
    .write = simple_proc_write,
    .release = simple_proc_release
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
