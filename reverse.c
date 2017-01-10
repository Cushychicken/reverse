#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nash Reilly <nrc.reilly@gmail.com");
MODULE_DESCRIPTION("Phrase reverser kernel module");

static unsigned long buffer_size = 8192;
module_param(buffer_size, ulong, (S_IRUSR | S_IRGRP | S_IROTH));
MODULE_PARM_DESC(buffer_size, "Internal buffer size");

static struct miscdevice reverse_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "reverse",
    .fops  = &reverse_fops
}

static int __init reverse_init(void)
{
    if (!buffer_size)
        return -1;

    misc_register(&reverse_misc_device);

    printk(KERN_INFO 
           "reverse device has been registered, buffer size is %lu bytes\n",
           buffer_size);
    
    return 0;
}

static void __exit reverse_exit(void)
{
    misc_deregister(&reverse_misc_device);
    printk(KERN_INFO "reverse device has been unregistered\n");
}

module_init(reverse_init);
module_exit(reverse_exit);

