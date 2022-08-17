#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>


static int hello_probe(struct platform_device *pdev)
{
	printk("%s %d, %d %s, match ok\n", __func__, __LINE__, current->pid, current->comm);
	return 0;
}

static int hello_remove(struct platform_device *pdev)
{
  printk("%s %d, %d %s\n", __func__, __LINE__, current->pid, current->comm);
	return 0;
}
 
static struct platform_driver hello_driver=
{
    .driver = {
		.name = "mali-rtc"
	},
    .probe = hello_probe,
    .remove = hello_remove,
};
 
 
static int hello_init(void)
{
	printk("%s %d, %d %s\n", __func__, __LINE__, current->pid, current->comm);
	return platform_driver_register(&hello_driver);
}
 
static void hello_exit(void)
{
	printk("%s %d, %d %s\n", __func__, __LINE__, current->pid, current->comm);
	platform_driver_unregister(&hello_driver);
	return;
}
 
MODULE_LICENSE("GPL");
module_init(hello_init);
module_exit(hello_exit);

#if 0
struct platform_driver {
	/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
	int (*probe)(struct platform_device *);
	int (*remove)(struct platform_device *);
	void (*shutdown)(struct platform_device *);
	int (*suspend)(struct platform_device *, pm_message_t state);
	/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
	int (*resume)(struct platform_device *);
	struct device_driver driver; //...
};
	
struct device_driver {
	const char              *name; /* 设备驱动程序的名称 */
	struct bus_type         *bus;

	struct module           *owner;
	const char              *mod_name;      /* used for built-in modules */

	bool suppress_bind_attrs;       /* disables bind/unbind via sysfs */
	enum probe_type probe_type;

	const struct of_device_id       *of_match_table;
	const struct acpi_device_id     *acpi_match_table;

	/* 这五个回调函数用于处理热插拔、即插即用和电源管理 */
	int (*probe) (struct device *dev);
	void (*sync_state)(struct device *dev);
	int (*remove) (struct device *dev);
	void (*shutdown) (struct device *dev);
	int (*suspend) (struct device *dev, pm_message_t state);
	int (*resume) (struct device *dev);

	const struct attribute_group **groups;
	const struct attribute_group **dev_groups;

	const struct dev_pm_ops *pm;
	void (*coredump) (struct device *dev);

	struct driver_private *p;
};
#endif
