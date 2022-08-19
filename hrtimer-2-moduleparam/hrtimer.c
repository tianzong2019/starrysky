#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>


/* 内核配置打开timers子系统中的高精度定时器
 * general-setup -》》timer subsys -》》 high resolution timer support
 *
 * 参数传递过程
 * sudo insmod param.ko value=100 value_out=200 string="gatieme" buffer="Hello-World" array=100,200,300
 *
 * 动态修改模块参数
 * 首先查看一下sysfs目录下的本模块参数信息: ls /sys/module/param/parameters
 *
 * 示例
 * [root@/mnt] # ls /sys/module/hrtimer/parameters
 *	array      buffer     string     value      value_out
 *
 * echo "hello" > string
 */

static struct hrtimer timer;
ktime_t               kt;
int                   cnt = 0;struct timeval        tv;


/*
 *  在模块里面, 声明一个变量（全局变量）,
 *  用来接收用户加载模块哦时传递的参数
 *
 *  module_param(name, type, perm)
 **/
static int value = 0;
module_param(value, int, 0644);
MODULE_PARM_DESC(value_int, "Get an value from user...\n");

/*
 *  在模块内部变量的名字和加载模块时传递的参数名字不同
 *
 *  module_param_named(name_out, name_in, type, perm)
 *
 *  @name_out    加载模块时，参数的名字
 *  @name_in     模块内部变量的名字
 *  @type        参数类型
 *  @perm        访问权限
 * */
static int  value_in = 0;
module_param_named(value_out, value_in, int, 0644);
MODULE_PARM_DESC(value_in, "value_in named var_out...\n");


/*
 *  加载模块的时候, 传递字符串到模块的一个全局字符数组里面
 *
 *  module_param_string(name, string, len, perm)
 *
 *  @name   在加载模块时，参数的名字
 *  @string 模块内部的字符数组的名字
 *  @len    模块内部的字符数组的大小
 *  #perm   访问权限
 *
 * */
static char *string = NULL;
module_param(string, charp, 0644);
MODULE_PARM_DESC(string, "Get an string(char *) value from user...\n");


static char buffer[20] = "gatieme";
module_param_string(buffer, buffer, sizeof(buffer), 0644);
MODULE_PARM_DESC(value_charp, "Get an string buffer from user...\n");


/*
 *  加载模块的时候, 传递参数到模块的数组中
 *
 *  module_param_array(name, type, num_point, perm)
 *
 *  @name       模块的数组名，也是外部制定的数组名
 *  @type       模块数组的数据类型
 *  @num_point  用来获取用户在加载模块时传递的参数个数,
 *              为NULL时，表示不关心用户传递的参数个数
 *  @perm       访问权限
 *
 * */
static int  array[3];
int         num;
module_param_array(array, int, &num, 0644);
MODULE_PARM_DESC(array, "Get an array from user...\n");

/* 定时器回调函数 */
static enum hrtimer_restart hrtimer_hander(struct hrtimer *timer)
{
	int index = 0;
	#if 0
	do_gettimeofday(&tv);
	if (cnt % 200 == 0)
		printk("%s %d, %d %s, %ld\n", __func__, __LINE__, current->pid, current->comm, jiffies);
	cnt++;
	#else
	mdelay(1000);
	printk("%s %d, %d %s, %ld\n", __func__, __LINE__, current->pid, current->comm, jiffies);
	#endif

	printk("\n---------------------\n");
    printk("value       : %d\n", value);
    printk("value_in    : %d\n", value_in);
    printk("string      : %s\n", string);
    printk("buffer      : %s\n", buffer);

    for(index = 0; index < num; index++)
    {
    printk("array[%2d]   :   %d\n", index, array[index]);
    }

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
