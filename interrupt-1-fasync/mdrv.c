#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/hrtimer.h>
#include <linux/delay.h>

static struct hrtimer timer;
ktime_t kt;
struct fasync_struct *async_queue; /* 异步相关结构体 */

/* 定时器回调函数 */
static enum hrtimer_restart hrtimer_hander_interrupt(struct hrtimer *timer)
{
	mdelay(1000);
	printk("%s %d, %d %s\n", __func__, __LINE__, current->pid, current->comm);

	/* 设置下次过期时间, 单位为秒和纳秒组合
	 * 1,000 皮秒 = 1纳秒 ns
	 * 1,000,000 皮秒 = 1微秒 us
	 * 1,000,000,000 皮秒 = 1毫秒 ms
	 * 1,000,000,000,000 皮秒 = 1秒 s
	 * ktime_t ktime_set(const long secs, const unsigned long nsecs)；
	 */
	kt = ktime_set(0, 1000000);   // 1ms

	/* 设置超时时间，当定时器超时后可以用该函数设置下一次超时时间
	 * hrtimer_forward_now(struct hrtimer *timer, ktime_t interval)
	 */
	hrtimer_forward_now(timer, kt);
	
	/* void kill_fasync(struct fasync_struct **fp, int sig, int band)
	 * fp: 要操作的文件指针
	 * sig：要发送的信号
	 * band：可读时设置为 POLL IN，可写时设置为 POLL OUT
	 */
	kill_fasync(&async_queue, SIGIO, POLL_IN);

	/* 该参数将重新启动定时器 */
	return HRTIMER_RESTART;
}

/* int (*fasync) (int fd, struct file *filp, int on) */
static int demo_fasync(int fd, struct file *filp, int on)
{
	/* int fasync_helper(int fd, struct file * filp, int on, struct fasync_struct **fapp) */
     if (fasync_helper(fd, filp, on, &async_queue) < 0) 
		return -EIO; 
     return 0; 
 } 

static int dmisc_open(struct inode *inode, struct file *filp)
{
	printk ("%s %d\n", __func__, __LINE__);
	/* hrtimer启动
	 * hrtimer_start(struct hrtimer *timer, ktime_t tim, const enum hrtimer_mode mode);
	 */
	hrtimer_start(&timer, kt, HRTIMER_MODE_REL);
    return 0;
}
static int dmisc_release(struct inode *inode, struct file *filp)
{
    printk ("%s %d\n", __func__, __LINE__);
	demo_fasync(-1, filp, 0); /* 删除异步通知 */
    return 0;
}
static ssize_t dmisc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    printk ("%s %d\n", __func__, __LINE__);
    return 0;
}
static ssize_t dmisc_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    printk ("%s %d\n", __func__, __LINE__);
    return 0;
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
	.fasync          = demo_fasync,
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
	/* hrtimer初始化 */
	kt = ktime_set(1, 10);
	hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	timer.function = hrtimer_hander_interrupt;  /* 设置回调函数 */
	return misc_register(&misc);;
}

static void hello_exit(void)
{
	printk ("%s %d\n", __func__, __LINE__);
	/* hrtimer注销 */
	hrtimer_cancel(&timer);
	misc_deregister(&misc);
	return;
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
