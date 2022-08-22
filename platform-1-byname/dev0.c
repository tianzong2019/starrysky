#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
 
#define prki(fmt,arg...) printk(KERN_INFO "%s %d, %d %s, " fmt, __FUNCTION__, __LINE__, current->pid, current->comm, ##arg)
 
static void dev_release(struct device *dev)
{
	prki("dev_release\n");
	return ;
}
 
static struct platform_device dev_device=
{
	.name = "mali-rtc",
    .id = -1,
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

#if 0
struct platform_device {
	const char * name; //设备名称
	int id;
	struct device dev;
	u32 num_resources; //设备使用各类资源的数量
	struct resource * resource; //设备使用的资源
	
	struct platform_device_id *id_entry;
};
#endif
