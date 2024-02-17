#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/types.h>

#include "rootkit.h"

#define OURMODNAME	"rootkit"

MODULE_AUTHOR("FOOBAR");
MODULE_DESCRIPTION("FOOBAR");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("0.1");

static int __init rootkit_init(void)
{
  printk(KERN_INFO "hello, world!\n");
	return 0;
}

static void __exit rootkit_exit(void)
{
	printk(KERN_INFO "Goodbye, world!\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit);