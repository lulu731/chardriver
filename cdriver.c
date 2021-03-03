#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include "cdriver.h"
#include <linux/string.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("lulu");
MODULE_DESCRIPTION("1st char driver");

static int dev_major = CDRIVER_MAJOR;
struct cdriver lulu_dev;
static int buff_size = 10;
//static int is_buf_size_changed = 0;

int set_device_major(struct cdriver *device)
{
    int result;
    result = 0;
    if (dev_major)
    {
        device->dev = MKDEV(dev_major, 0);
        result = register_chrdev_region(device->dev, 1, "luludriver");
    } else {
        result = alloc_chrdev_region(&device->dev, 0, 1, "luludriver");
    }
    if (result < 0)
    {
        printk(KERN_WARNING "luludriver: can't get major %d\n,", dev_major);
    }
    return result;
}

static ssize_t read_fn(struct file *file, char *buf, size_t count, loff_t *ppos)
{
    ssize_t retval;
    struct cdriver *dev;
    
//    printk(KERN_WARNING "count : %ld\n", count);
    //printk(KERN_WARNING "ppos : %lld\n", *ppos);

    dev = file->private_data;
    
    //printk(KERN_WARNING "buffer %s\n", dev->buffer);
    retval = copy_to_user(buf, dev->buffer, dev->data_len);//buff_size);
    
//    printk(KERN_WARNING "In read, retval : %ld\n", retval);

    if (*ppos < dev->data_len)
    {
        *ppos += dev->data_len-retval;
        return dev->data_len-retval;
    }
    else
    {
        return 0;
    }
}

static ssize_t write_fn(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
    ssize_t retval;
    struct cdriver *dev;
    
    

//    printk(KERN_WARNING "count : %ld\n", count);
    //printk(KERN_WARNING "ppos : %lld\n", *ppos);

    dev = file->private_data;
    
    if (mutex_lock_interruptible(&lulu_dev.mut))
    {
        return -ERESTARTSYS;
    }
    
    
    retval = copy_from_user(dev->buffer, buf, count);
    mutex_unlock(&lulu_dev.mut);
    //printk(KERN_WARNING "In write, retval : %ld\n", retval);
  //  printk(KERN_WARNING "buffer %s\n", dev->buffer);
    //printk(KERN_WARNING "ppos : %lld\n", *ppos);
    *ppos += count;
    dev->data_len = *ppos;
    //printk(KERN_WARNING "ppos : %lld\n", *ppos);
    return count-retval;
}

static long ioctl_fn(struct file *file, unsigned int cmd, unsigned long arg)
{
    long retval = 0;
    //printk(KERN_WARNING "in ioctl\n");
    switch (cmd)
    {
    case CDRIVER_IOCSBUFF_SIZE:
        //printk(KERN_WARNING "in IOCSBUF\n");
        //printk(KERN_WARNING "buff_size before = %d\n", buff_size);
        if (!capable(CAP_SYS_ADMIN))
        {
            return -EPERM;
        }
        retval = __get_user(buff_size, (int __user *)arg);
        //printk(KERN_WARNING "buff_size after= %d\n", buff_size);
        lulu_dev.buffer = krealloc(lulu_dev.buffer, buff_size * sizeof(char), GFP_KERNEL);
        //printk(KERN_WARNING "in IOCSBUF %ld\n", retval);
        break;
    
    case CDRIVER_IOCQBUFF_SIZE:
        //printk(KERN_WARNING "buff_size before = %d\n", buff_size);
        retval = (long) buff_size;
        //printk(KERN_WARNING "in IOCQBUF %ld\n", retval);
        //printk(KERN_WARNING "buff_size after= %d\n", buff_size);
        break;

    default:
        return -ENOTTY;
        break;
    }
    return retval;
}

static int open_fn(struct inode *inode, struct file *file)
{
    struct cdriver *dev;
    printk(KERN_WARNING "in open");
    dev = container_of(inode->i_cdev, struct cdriver, cdev);
    file->private_data = dev;
    return 0;
}

static int release_fn(struct inode *inode, struct file *file)
{
    printk(KERN_WARNING "in release");
    //file->private_data = NULL;
    return 0;
}


struct file_operations fops =
{
    .owner = THIS_MODULE,
    .read = read_fn,
    .write = write_fn,
    .open = open_fn,
    .release = release_fn,
    .unlocked_ioctl = ioctl_fn
};

static int __init driv_init(void)
{
    int err, devno ;
    int major;

    mutex_init(&lulu_dev.mut);

    set_device_major(&lulu_dev);
    major = MAJOR(lulu_dev.dev);
    
    devno = MKDEV(major, 0);
    cdev_init(&lulu_dev.cdev, &fops);
    lulu_dev.cdev.owner = THIS_MODULE;
    lulu_dev.cdev.ops = &fops;
    err = cdev_add(&lulu_dev.cdev, devno, 1);
    
    if (err)
    {
        printk(KERN_NOTICE "Error %d adding luludevice", err);
    } 

    lulu_dev.buffer = kmalloc(buff_size * sizeof(char), GFP_KERNEL);
//todo : see kmalloc with zeros
    memzero_explicit(lulu_dev.buffer, buff_size);
    strcpy(lulu_dev.buffer, "lulu");
    printk(KERN_ALERT "major = %d\n", major); 
    return err;
};

static void __exit driv_exit(void)
{    
    kfree(lulu_dev.buffer);
    lulu_dev.buffer = NULL;
    unregister_chrdev_region(lulu_dev.dev, 1);
    cdev_del(&lulu_dev.cdev);
    printk(KERN_ALERT "Bye bye!\n");
};

module_init(driv_init);
module_exit(driv_exit);