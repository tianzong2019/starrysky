#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/jiffies.h>
#include <linux/delay.h>

/* 内核配置打开timers子系统中的高精度定时器
 * general-setup -》》timer subsys -》》 high resolution timer support
 */

static struct hrtimer timer;
ktime_t               kt;
int                   cnt = 0;struct timeval        tv;

/* 定时器回调函数 */
static enum hrtimer_restart hrtimer_hander(struct hrtimer *timer)
{
	#if 0
	do_gettimeofday(&tv);
	if (cnt % 200 == 0)
		printk("%s %d, %d %s, %ld\n", __func__, __LINE__, current->pid, current->comm, jiffies);
	cnt++;
	#else
	mdelay(1000);
	printk("%s %d, %d %s, %ld\n", __func__, __LINE__, current->pid, current->comm, jiffies);
	#endif

	/* 设置下次过期时间, 单位为秒和纳秒组合
	 * 1,000 皮秒 = 1纳秒 ns
	 * 1,000,000 皮秒 = 1微秒 us
	 * 1,000,000,000 皮秒 = 1毫秒 ms
	 * 1,000,000,000,000 皮秒 = 1秒 s
	 *
	 * 1,000 ns = 1us
	 * 1,000,000 ns = 1ms
	 * 1,000,000,000 ns = 1s
	 *
	 * ktime_t ktime_set(const long secs, const unsigned long nsecs)；
	 */
	kt = ktime_set(0, 1000000);   // 1ms

	/* 设置超时时间，当定时器超时后可以用该函数设置下一次超时时间
	 * hrtimer_forward_now(struct hrtimer *timer, ktime_t interval)
	 */
	hrtimer_forward_now(timer, kt);

	/* 该参数将重新启动定时器 */
	return HRTIMER_RESTART;
}

static int __init hrtimer_demo_init(void)
{
	printk("%s %d, %d %s\n", __func__, __LINE__, current->pid, current->comm);

	kt = ktime_set(1, 10);

	/* hrtimer初始化
	 * void hrtimer_init(struct hrtimer *timer, clockid_t clock_id, enum hrtimer_mode mode);
	 */
	hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

	/* hrtimer启动
	 * hrtimer_start(struct hrtimer *timer, ktime_t tim, const enum hrtimer_mode mode);
	 */
	hrtimer_start(&timer, kt, HRTIMER_MODE_REL);

	/* 设置回调函数 */
	timer.function = hrtimer_hander;

	return 0;
}

static void __exit hrtimer_demo_exit(void)
{
	/* hrtimer注销 */
	hrtimer_cancel(&timer);
	printk("%s %d, %d %s\n", __func__, __LINE__, current->pid, current->comm);
}

module_init(hrtimer_demo_init);
module_exit(hrtimer_demo_exit);
MODULE_LICENSE("GPL");

#if 0

void mdelay(unsigned long msecs)  毫秒 1ms
void udelay(unsigned long usecs)  微秒 1ms = 1000us
void ndelay(unsigned long nsecs)  纳秒 1ms = 1000,000ns

jiffies类型为无符号长整型(unsigned long)，其他任何类型存放它都不正确。

将以秒为单位的时间转化为jiffies：seconds * Hz

将jiffies转化为以秒为单位的时间：jiffies / Hz

相比之下，内核中将秒转换为jiffies用的多些。
#endif
