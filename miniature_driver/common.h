#ifndef __COMMON_H_
#define __COMMON_H_

struct mini_device {
	struct wait_queue_head key_waitq;//定义队列
	struct hrtimer timer;
	unsigned char ev_press;
	char value;
	ktime_t kt;
};

#endif
