#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
 
#define prki(fmt,arg...) printk(KERN_INFO "%s %d, %d %s, " fmt, __FUNCTION__, __LINE__, current->pid, current->comm, ##arg)
 
static void dev_release(struct device *dev)
{
	prki("dev_release\n");
	return ;
}

static struct resource rtc_resource[] = {
	[0] = {
			.start  = 0x10,
			.end    = 0x10 + 0xff,
			.flags  = IORESOURCE_MEM,
	},
	[1] = {
			.start  = 0x20,
			.end    = 0x20,
			.flags  = IORESOURCE_IRQ,
	},
	[2] = {
			.start  = 0x30,
			.end    = 0x30,
			.flags  = IORESOURCE_IRQ
	}
};

struct platform_device dev_device = {
    .name = "s3c2410-rtc",
    .id = -1,
    .num_resources = ARRAY_SIZE(rtc_resource),
    .resource = rtc_resource,
	.dev.release = dev_release,
};

static int dev_init(void)
{
	prki("dev_init\n");
	return platform_device_register(&dev_device);
}
 
static void dev_exit(void)
{
	prki("dev_exit\n");
	platform_device_unregister(&dev_device);
	return;
}
 
MODULE_LICENSE("GPL");
module_init(dev_init);
module_exit(dev_exit);
