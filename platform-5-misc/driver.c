#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>


static int hello_open(struct inode *inode, struct file *file)
{
    printk("%s %d, %d %s\n", __func__, __LINE__, current->pid, current->comm);
    return 0;
}

static int hello_close(struct inode *inode, struct file *file)
{
    printk("%s %d, %d %s\n", __func__, __LINE__, current->pid, current->comm);
    return 0;
}

static const struct file_operations hello_ops = {
    .owner = THIS_MODULE,
    .open = hello_open,
    .release = hello_close,
};

static struct miscdevice hello_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "hello",
    .fops = &hello_ops,
};

static int hello_probe(struct platform_device *pdev)
{
    int ret = 0;

    printk("%s %d, %d %s, match ok\n", __func__, __LINE__, current->pid, current->comm);

    ret = misc_register(&hello_misc_device);
    if (0 != ret)
    {
        pr_err("could not register the misc device\n");
        return ret;
    }

    pr_info(">>>>> minor num:%d\n", hello_misc_device.minor);

    return 0;
}

static int hello_remove(struct platform_device *pdev)
{
    printk("%s %d, %d %s\n", __func__, __LINE__, current->pid, current->comm);
    misc_deregister(&hello_misc_device);

    return 0;
}

static struct platform_driver hello_driver = {
    .probe = hello_probe,
    .remove = hello_remove,
    .driver = {
        .name = "mali-rtc"
    },
};

module_platform_driver(hello_driver);
MODULE_LICENSE("GPL");
