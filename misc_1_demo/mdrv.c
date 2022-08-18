#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

static int gval = 0;

static int dmisc_open(struct inode *inode, struct file *filp)
{
    printk ("%s %d\n", __func__, __LINE__);
    return 0;
}
static int dmisc_release(struct inode *inode, struct file *filp)
{
    printk ("%s %d\n", __func__, __LINE__);
    return 0;
}
static ssize_t dmisc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int ret = 0;
    printk ("%s %d\n", __func__, __LINE__);
	ret = copy_to_user(buf, &gval, sizeof(gval));
    return ret;
}
static ssize_t dmisc_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	int ret = 0;
	ret = copy_from_user(&gval, buf, count);
    printk ("%s %d, gval %d\n", __func__, __LINE__, gval);
	gval += 10;

    return ret;
}
static long dmisc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    printk ("%s %d\n", __func__, __LINE__);
    return 0;
}
static struct file_operations dmisc_fops =
{
    .owner           = THIS_MODULE,
    .open            = dmisc_open,
    .read            = dmisc_read,
    .write           = dmisc_write,
    .compat_ioctl    = dmisc_ioctl,
    .release         = dmisc_release
};
static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "miscdev",
    .fops = &dmisc_fops,
};
static int hello_init(void)
{
	printk ("%s %d\n", __func__, __LINE__);
	return misc_register(&misc);;
}

static void hello_exit(void)
{
	printk ("%s %d\n", __func__, __LINE__);
	misc_deregister(&misc);
	return;
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
