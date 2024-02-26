#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>		/* file_operations */
#include <linux/fs_context.h> /* fs_context */

#define MYFS_MAGIC 0x880227FF

/******* functions ********/
static int __init myfs_init_fs(void);
static void __exit myfs_exit_fs(void);

static int myfs_open(struct inode *inode, struct file *filp);
static ssize_t myfs_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t myfs_write(struct file *filp, const char *buf, size_t len, loff_t * off);

static struct inode_operations myfs_inode_operations = {
	.lookup = simple_lookup,
	.link = simple_link,
};

static struct file_operations myfs_file_operations = {
	.open = myfs_open,
	.read = myfs_read,
	.write = myfs_write,
};


/**
 * ref: linux/fs/debugfs/inode.c
 * ref: linux/fs/ramfs/inode.c
*/
struct inode *myfs_get_inode(struct super_block *sb)
{
	pr_info("myfs: %s: start to allocate inode\n", __func__);
	
	struct inode * inode = new_inode(sb);

	pr_info("myfs: %s: end to allocate inode\n", __func__);

	if (inode) {
		inode->i_ino = 0;
		inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
		inode->i_op = &myfs_inode_operations;
		inode->i_fop = &myfs_file_operations;
		inode->i_mode = S_IFDIR | 0755;
	}

	pr_info("myfs: %s: finished\n", __func__);
	return inode;
}

static struct super_operations myfs_super_operations = {
	.statfs = simple_statfs,
};

/**
 * ref: linux/fs/debugfs/inode.c
 * ref: linux/fs/ramfs/inode.c
 * ref: linux/fs/proc/root.c
*/
static int myfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;

	sb->s_blocksize		= 1024;
	sb->s_blocksize_bits	= 10;
	sb->s_magic = MYFS_MAGIC;
  sb->s_op = &myfs_super_operations;
	sb->s_time_gran		= 1;

	/* procfs dentries and inodes don't require IO to create */
	sb->s_shrink.seeks = 0;

	pr_info("myfs: %s: start to get inode\n", __func__);

	inode = myfs_get_inode(sb);
	if (!inode) {
		pr_err("myfs: %s: get root inode failed\n", __func__);
		return -ENOMEM;
	}

	pr_info("myfs: %s: d_make_root starts\n", __func__);

	sb->s_root = d_make_root(inode);
	if (!sb->s_root) {
		pr_err("myfs: %s: allocate root inode failed\n", __func__);
		return -ENOMEM;
	}

	pr_info("myfs: %s: d_make_root finished\n", __func__);

	return 0;
}

static int myfs_open(struct inode *inode, struct file *filp)
{
	pr_info("myfs: %s\n", __func__);
	return 0;
}

static ssize_t myfs_read(struct file *filp, char __user *buf, size_t len,loff_t * off)
{
	pr_info("myfs: %s\n", __func__);
	return 0;
}

static ssize_t myfs_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{
	pr_info("myfs: %s\n", __func__);
	return 0;
}

static struct dentry *myfs_mount(struct file_system_type *fs_type,
			int flags, const char *dev_name,
			void *data)
{
	return mount_single(fs_type, flags, data, myfs_fill_super);
}

static struct file_system_type myfs_type = {
	.owner = THIS_MODULE,
	.name = "myfs",
	.mount = myfs_mount,
	.kill_sb = kill_block_super,
	.fs_flags = FS_USERNS_MOUNT
};

/**
 * ref: linux/fs/debugfs/inode.c
*/
static int __init myfs_init_fs(void)
{
	int retval;

	retval = register_filesystem(&myfs_type);

	return retval;
}

static void __exit myfs_exit_fs(void)
{
	unregister_filesystem(&myfs_type);
}

module_init(myfs_init_fs);
module_exit(myfs_exit_fs);

MODULE_AUTHOR("JACKERYLI");
MODULE_DESCRIPTION("JACKERYLI");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("0.1");
