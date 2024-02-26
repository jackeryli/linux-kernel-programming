#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>		/* file_operations */
#include <linux/cdev.h>		/* character device */
#include <linux/device.h>	/* create_class */
#include <linux/err.h>		/* IS_ERR */
#include <linux/slab.h>		/* kmalloc */
#include <linux/uaccess.h>	/* copy_to_user */
#include <linux/mutex.h>	/* mutex */

#define KBUFSIZE 1024

dev_t dev = 0;
static struct cdev *kernel_cdev;
static struct class *dev_class;
char* kernel_buf;
struct mutex jackdriver_mutex;

/******* functions ********/
static int __init jackdriver_init(void);
static void __exit jackdriver_exit(void);

static int jackdriver_open(struct inode *inode, struct file *filp);
static ssize_t jackdriver_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t jackdriver_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static int jackdriver_release(struct inode *inode, struct file *filp);

const struct file_operations fops = {
    	open:jackdriver_open,
    	read:jackdriver_read,
        write:jackdriver_write,
    	release:jackdriver_release,
    	owner:THIS_MODULE
};

static int jackdriver_open(struct inode *inode, struct file *filp)
{
	pr_info("jackdriver: %s\n", __func__);
	return 0;
}

static ssize_t jackdriver_read(struct file *filp, char __user *buf, size_t len,loff_t * off)
{
	ssize_t ret = 0;

	if(copy_to_user(buf, kernel_buf, KBUFSIZE))
	{
		pr_err("jackdriver: read error\n");
		ret = -EFAULT;
	} else {
		pr_info("jackdriver: read finished\n");
		ret = KBUFSIZE;
	}
	
	return ret;
}

static ssize_t jackdriver_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{	
	ssize_t ret = 0;

	/* Prevent threads writing to same buf at the same time */
	mutex_lock(&jackdriver_mutex);
	
	/* Clear kernel_buf to zero */
	memset(kernel_buf, 0, KBUFSIZE);
	
	if(copy_from_user(kernel_buf, buf, len))
	{
		pr_err("jackdriver: write error\n");
		ret = -EFAULT;
	} else {
		pr_info("jackdriver: write finished\n");
		ret = len;
	}
	
	mutex_unlock(&jackdriver_mutex);
	
	return ret;
}

static int jackdriver_release(struct inode *inode, struct file *filp)
{
	pr_info("jackdriver: %s\n", __func__);
	return 0;
}

static int __init jackdriver_init(void)
{
        int ret;

	/* Allocate major number */
	ret = alloc_chrdev_region(&dev, 0, 1, "jackdriver");
	if (ret < 0) {
		pr_info("jackdriver: major number allocation failed\n");
		return ret;
	}

	pr_info("jackdriver: major=%d, minor=%d\n", MAJOR(dev), MINOR(dev));

	/* Create cdev */
	kernel_cdev = cdev_alloc();
	kernel_cdev->ops = &fops;
	kernel_cdev->owner = THIS_MODULE;

	/* Allocate cdev */
	ret = cdev_add(kernel_cdev, dev, 1);
	if (ret < 0) {
		pr_info("jackdriver: unable to allocate cdev\n");
		goto error;
	}

	mutex_init(&jackdriver_mutex);

	/* Allocate Kernel Buffer */
	kernel_buf = kmalloc(KBUFSIZE, GFP_KERNEL);
	if (kernel_buf == 0) {
		pr_info("jackdriver: unable to allocate kernel buffer\n");
		goto error;
	}

	strcpy(kernel_buf, "Hello Jack!");

	pr_info("jackdriver: init finished\n");
	return 0;

error:
	unregister_chrdev_region(dev, 1);
	return ret;
}

static void __exit jackdriver_exit(void)
{
	mutex_destroy(&jackdriver_mutex);
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(kernel_cdev);
	unregister_chrdev_region(dev, 1);
}

module_init(jackdriver_init);
module_exit(jackdriver_exit);

MODULE_AUTHOR("JACKERYLI");
MODULE_DESCRIPTION("JACKERYLI");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("0.1");
