#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/hrtimer.h>
#include <linux/delay.h>
#include <linux/poll.h>

static struct hrtimer timer;
ktime_t kt;
static unsigned char ev_press = 0;
static DECLARE_WAIT_QUEUE_HEAD(key_waitq);//定义队列
static char value = 0;

/* 定时器回调函数 */
static enum hrtimer_restart hrtimer_hander_interrupt(struct hrtimer *timer)
{
	value = value ? 0 : 1;
	printk("%s %d, %d %s， 0x%x\n", __func__, __LINE__, current->pid, current->comm, value);

	/* 设置下次过期时间, 单位为秒和纳秒组合
	 * 1,000 皮秒 = 1纳秒 ns
	 * 1,000,000 皮秒 = 1微秒 us
	 * 1,000,000,000 皮秒 = 1毫秒 ms
	 * 1,000,000,000,000 皮秒 = 1秒 s
	 * ktime_t ktime_set(const long secs, const unsigned long nsecs)；
	 */
	kt = ktime_set(1, 0);   // 1s

	/* 设置超时时间，当定时器超时后可以用该函数设置下一次超时时间
	 * hrtimer_forward_now(struct hrtimer *timer, ktime_t interval)
	 */
	hrtimer_forward_now(timer, kt);
	
	
	ev_press = 1;
	wake_up_interruptible(&key_waitq);/* 唤醒休眠的进程 */

	/* 该参数将重新启动定时器 */
	return HRTIMER_RESTART;
}

static unsigned int key_poll (struct file *file, struct poll_table_struct *wait)
{
	unsigned int mask = 0;
	/* 将进程挂在key_waitq队列上 */  
	poll_wait(file, &key_waitq, wait);
 
	if(ev_press == 1)
		mask |= POLLIN | POLLRDNORM;/* 表示有普通数据可读 */

	return mask;
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
    return 0;
}
static ssize_t dmisc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
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
	wait_event_interruptible(key_waitq, ev_press);
	if(copy_to_user(buf, &value, 1)){
		printk("copy_to_user error");
	}
	
	ev_press = 0;//清零

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
	.poll            = key_poll,
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

#if 0
poll常用宏定义以及其含义

#define POLLIN		0x0001  //有数据可读。
#define POLLPRI		0x0002	//有紧迫数据可读。
#define POLLOUT		0x0004	//写数据不会导致阻塞。
#define POLLERR		0x0008	//指定的文件描述符发生错误。
#define POLLHUP		0x0010	//指定的文件描述符挂起事件。
#define POLLNVAL	0x0020  //指定的文件描述符非法。
#define POLLRDNORM	0x0040  //有普通数据可读。
#define POLLRDBAND	0x0080  //有优先数据可读。
#define POLLWRNORM	0x0100  //写普通数据不会导致阻塞。
#define POLLWRBAND	0x0200  //写优先数据不会导致阻塞。
#define POLLMSG		0x0400  //消息可用。
#define POLLREMOVE	0x1000  //删除指定的文件描述符



在Linux驱动程序中可以用等待队列来实现阻塞进程的唤醒

利用宏：DECLARE_WAIT_QUEUE_HEAD(button_queue);//定义并初始化一个名为button_queue的等待队列(等待队列的定义和初始化有具体的函数，这个宏也是替换成或调用哪些函数)。

等待事件：
wait_event(queue);
wait_event_interruptile(queue,condition);//queue是等待被唤醒的队列，condition是唤醒的条件，不满足就继续阻塞。
                                         //使调用进程在等待队列上睡眠，直到满足给定condition条件为止

唤醒队列:
wake_up_interruptible(&queue);//唤醒queue等待队列中的进程

wait_event_interruptile 和 wake_up_interruptible一般成对使用。


poll_wait()是一个非阻塞函数
- 使用这个函数一般要配合等待队列使用，poll_wait()函数的作用是将对应的等待队列头添加到poll_table，
- 有人会问如果驱动的poll()函数不调用poll_wait()，即屏蔽掉该行会怎么样？
- 答案是应用层poll函数会一直休眠下去直到它超时，可以参考下面第四节实验验证部分。
- 原因这里可以简单说下，以按键驱动为例，按键按下触发中断，按键值被更新，屏蔽掉poll_wait()后，应用进程还是一直在休眠状态无法立即从休眠状态变为可执行态并被调度执行，直到poll函数超时。
- 我们通过wake_up_interruptible()将等待队列中的进程变为可运行态，这样刚才调用poll_wait()函数其对应的应用进程才会被调度执行。


实验 1，屏蔽poll_wait()函数
- 可以看到屏蔽poll_wait函数会导致应用进程无法第一时间被执行，屏蔽poll_wait函数后硬件中断到达后无法第一时间将应用进程变为可运行态，导致应用进程一直休眠直到poll函数超时。


实验 2，屏蔽wake_up_interruptible()函数
- 和实验1类似，应用程序无法第一时间得到调度。


实验 3，wait_event_interruptible()函数condition不满足
- 将condition置为假
- 只要响应一次，应用进程执行到Read函数会一直被堵塞住

#endif
