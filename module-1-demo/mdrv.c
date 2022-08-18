#include <linux/module.h>

static int hello_init(void)
{
	printk("hello_init\n");
	return 0;
}

static void hello_exit(void)
{
	printk("hello_exit\n");
	return;
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
