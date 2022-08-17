#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>

#include <linux/kthread.h>
#include <linux/delay.h>


static int demo_thr(void *data)
{
	while (!kthread_should_stop()) {
		printk("%s %d, %d %s\n", __func__, __LINE__, current->pid, current->comm);
		msleep_interruptible(2000);
	}
	return 0;
}

static struct task_struct *thr = NULL;

static int hello_probe(struct platform_device *pdev)
{
	printk("%s %d, %d %s, match ok\n", __func__, __LINE__, current->pid, current->comm);
	return 0;
}

static int hello_remove(struct platform_device *pdev)
{
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
	thr = kthread_run(demo_thr, NULL, "kthread-demo");
	return platform_driver_register(&hello_driver);
}
 
static void hello_exit(void)
{
	printk("%s %d, %d %s\n", __func__, __LINE__, current->pid, current->comm);
	if (thr)
		kthread_stop(thr);
	thr = NULL;
	platform_driver_unregister(&hello_driver);
	return;
}
 
MODULE_LICENSE("GPL");
module_init(hello_init);
module_exit(hello_exit);

#if 0
struct task_struct *kthread_create(int (*threadfn)(void *data), void *data, const char namefmt[], ...);

#define kthread_run(threadfn, data, namefmt, ...)
- 其中kthread_create()只是创建一个内核线程，但并没有启动
- 调用wake_up_process()来启动线程
- 内核又帮我们定义了一个宏kthread_run, 创建后直接启动

int kthread_stop(struct task_struct *k);
- 关闭线程
- 这个调用是会阻塞等待，直到内核线程k退出为止。
- 原因为因为此函数内部会调用wait_for_completion(）的方法（通过等待队列来实现），阻塞等待内核线程自身的退出。

int kthread_should_stop(void);
- 判断自身是否需要退出
- 如果该内核线程已经被设置stop标志了，则会返回1，否则返回0
#endif

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
