#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nash Reilly <nrc.reilly@gmail.com");
MODULE_DESCRIPTION("Phrase reverser kernel module");

static unsigned long buffer_size = 8192;
module_param(buffer_size, ulong, (S_IRUSR | S_IRGRP | S_IROTH));
MODULE_PARM_DESC(buffer_size, "Internal buffer size");

struct buffer {
    wait_queue_head_t read_queue;
    struct mutex lock;
    char *data, *end;
    char *read_ptr;
    unsigned long size;
};

static struct buffer *buffer_alloc(unsigned long size)
{
    struct buffer *buf;
    buf = kzalloc(sizeof(*buf), GFP_KERNEL);
    if (unlikely(!buf))
        goto out;

    buf->data = kzalloc(size, GFP_KERNEL);
    if (unlikely(!buf->data)) 
        goto out_free;

    init_waitqueue_head(&buf->read_queue);

    mutex_init(&buf->lock);

    buf->size = size;

out: 
    return buf;

out_free:
    kfree(buf);
    return NULL;
};

static void buffer_free(struct buffer *buffer)
{
    kfree(buffer->data);
    kfree(buffer);
};

static int reverse_open(struct inode *inode, struct file *file)
{
    struct buffer *buf;
    int err = 0;

    buf = buffer_alloc(buffer_size);
    if (unlikely(!buf)) {
        err = -ENOMEM;
        goto out;
    }

    file->private_data = buf;

out:
    return err;
}

static struct file_operations reverse_fops = {
    .owner  = THIS_MODULE,
    .open   = reverse_open,
    .llseek = noop_llseek
};

static struct miscdevice reverse_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "reverse",
    .fops  = &reverse_fops
};

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

