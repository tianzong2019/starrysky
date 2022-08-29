#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/hrtimer.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include "common.h"

struct mini_device *minidev = NULL;

/* 定时器回调函数 */
static enum hrtimer_restart hrtimer_hander_interrupt(struct hrtimer *timer)
{
	minidev->value = minidev->value ? 0 : 1;
	printk("%s %d, %d %s， 0x%x\n", __func__, __LINE__, current->pid, current->comm, minidev->value);

	minidev->kt = ktime_set(1, 0);   //单位为秒和纳秒组合，1s = 1000 000 000ns

	/* 设置超时时间，当定时器超时后可以用该函数设置下一次超时时间
	 * hrtimer_forward_now(struct hrtimer *timer, ktime_t interval)
	 */
	hrtimer_forward_now(timer, minidev->kt);
	
	
	minidev->ev_press = 1;
	wake_up_interruptible(&minidev->key_waitq);/* 唤醒休眠的进程 */

	/* 该参数将重新启动定时器 */
	return HRTIMER_RESTART;
}

static unsigned int event_poll (struct file *filp, struct poll_table_struct *wait)
{
	unsigned int mask = 0;
	struct miscdevice *lcmis = filp->private_data;
	struct mini_device *data = (struct mini_device *)dev_get_drvdata(lcmis->this_device);

	/* 将进程挂在key_waitq队列上 */  
	poll_wait(filp, &data->key_waitq, wait);
 
	if(data->ev_press == 1)
		mask |= POLLIN | POLLRDNORM;/* 表示有普通数据可读 */

	return mask;
}

static int dmisc_open(struct inode *inode, struct file *filp)
{
	struct miscdevice *lcmis = filp->private_data;
	struct mini_device *data = (struct mini_device *)dev_get_drvdata(lcmis->this_device);

	printk ("%s %d, %d\n", __func__, __LINE__, data->value);

	/* hrtimer启动 */
	hrtimer_start(&data->timer, data->kt, HRTIMER_MODE_REL);

    return 0;
}

static int dmisc_release(struct inode *inode, struct file *filp)
{
	struct miscdevice *lcmis = filp->private_data;
	struct mini_device *data = (struct mini_device *)dev_get_drvdata(lcmis->this_device);

    printk ("%s %d\n", __func__, __LINE__);
	/* hrtimer注销，与启动对应 */
	hrtimer_cancel(&data->timer);

    return 0;
}

static ssize_t dmisc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct miscdevice *lcmis = filp->private_data;
	struct mini_device *data = (struct mini_device *)dev_get_drvdata(lcmis->this_device);

    printk ("%s %d\n", __func__, __LINE__);
	/* 宏 wait_event_interruptible(wq, condition)
	 * 1) 条件condition为真时调用这个函数将直接返回0，而当前进程不会
	 *    被 wait_event_interruptible和从runqueue队列中删除。
	 * 2) 如果要被wait_event_interruptible的当前进程有nonblocked pending
     *    signals, 那么会直接返回-ERESTARTSYS(i.e. -512)，当前进程不会
     *    被wait_event_interruptible 和从runqueue队列中删除。
	 * 3) 其他情况下，当前进程会被正常的wait_event_interruptible，并从
     *    runqueue队列中删除，进入TASK_INTERRUPTIBLE状态退出运行调度，
     *    直到再次被唤醒加入runqueue队列中后而参与调度，将正常返回0。
	 */
	wait_event_interruptible(data->key_waitq, data->ev_press);
	if(copy_to_user(buf, &data->value, 1)){
		printk("copy_to_user error");
	}
	
	data->ev_press = 0;//清零

    return 0;
}

static ssize_t dmisc_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	struct miscdevice *lcmis = filp->private_data;
	struct mini_device *data = (struct mini_device *)dev_get_drvdata(lcmis->this_device);

    printk ("%s %d, %d\n", __func__, __LINE__, data->value);

    return 0;
}

static long dmisc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct miscdevice *lcmis = filp->private_data;
	struct mini_device *data = (struct mini_device *)dev_get_drvdata(lcmis->this_device);

    printk ("%s %d, %d\n", __func__, __LINE__, data->value);

    return 0;
}

static struct file_operations dmisc_fops =
{
    .owner           = THIS_MODULE,
    .open            = dmisc_open,
    .read            = dmisc_read,
    .write           = dmisc_write,
    .compat_ioctl    = dmisc_ioctl,
	.poll            = event_poll,
    .release         = dmisc_release
};

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "miscdev",
    .fops = &dmisc_fops,
};

static int hello_probe(struct platform_device *pdev)
{
	printk("%s %d, %d %s, match ok\n", __func__, __LINE__, current->pid, current->comm);
	misc_register(&misc);

	minidev = devm_kzalloc(&pdev->dev, sizeof(*minidev), GFP_KERNEL);

	minidev->value = 19;
	init_waitqueue_head(&minidev->key_waitq);

	/* hrtimer初始化 */
	minidev->kt = ktime_set(1, 10);
	hrtimer_init(&minidev->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	minidev->timer.function = hrtimer_hander_interrupt;  /* 设置回调函数 */
	
	dev_set_drvdata(misc.this_device, minidev);
	dev_set_drvdata(&pdev->dev, minidev);

	return 0;
}

static int hello_remove(struct platform_device *pdev)
{
	struct mini_device *data = (struct mini_device *)dev_get_drvdata(&pdev->dev);

	printk("%s %d, %d %s, %d\n", __func__, __LINE__, current->pid, current->comm, data->value);
	misc_deregister(&misc);

	return 0;
}

static struct platform_device_id rtc_driver_ids[] = {
	{
		.name		= "s3c2410-rtc",   // to match
		.driver_data	= 0,
	}, {
		.name		= "s3c2416-rtc",
		.driver_data	= 1,
	}, {
		.name		= "s3c2443-rtc",
		.driver_data	= 2,
	}, {
		.name		= "s3c64xx-rtc",
		.driver_data	= 3,
	},
	{}
};

static struct of_device_id led_of_match[] = {
    {.compatible = "s3c,led0", .data = (void *)0},
    {.compatible = "s3c,led1", .data = (void *)0},
    {},
};

static struct platform_driver hello_driver = {
    .driver = {
		.name = "mali-rtc",  /* must be set */
		.of_match_table = led_of_match,
	},
	.id_table	= rtc_driver_ids,
    .probe = hello_probe,
    .remove = hello_remove,
};

module_platform_driver(hello_driver);
MODULE_LICENSE("GPL");
