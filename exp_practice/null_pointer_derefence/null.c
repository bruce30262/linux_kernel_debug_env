#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
 
void (*my_funptr)(void);
 
ssize_t bug1_write(struct file *file, const char *buf, unsigned long len)
{
    my_funptr();
    return len;
}
 
struct file_operations fops = {
    .write = bug1_write,
};

static int __init null_dereference_init(void)
{
    printk(KERN_INFO "null_dereference driver init!\n");

    if (! proc_create("bug1",0666,NULL,&fops)) {
        printk(KERN_ALERT "ERROR! proc_create\n");
        remove_proc_entry("bug1",NULL);
        return -1;
    }
    return 0;
}
 
static void __exit null_dereference_exit(void)
{
    printk(KERN_INFO "null_dereference driver exit\n");
}
 
module_init(null_dereference_init);
module_exit(null_dereference_exit);
