#include <linux/module.h>
#include <linux/slab.h>
#include "kqueue.h"

struct hellomsg {
	struct list_head node;
	int id;
	int size;
	void *vaddr;
};

struct queuelist qlst;

static int hello_init(void)
{
	
	struct hellomsg *info = NULL;
	int i=0;
	
	printk("hello_init\n");
	queue_init(&qlst, 10);
	
	printk("queue %d %d\n", qlst.cur, queue_len(&qlst));
	
	for(i=0;i<10;i++){
		info = kzalloc(sizeof(*info),GFP_KERNEL);
		info->id = i+10;
		enqueue(&qlst, info);
		printk("enqueue %d, %d\n", info->id, queue_len(&qlst));
		kfree(info);
		info=NULL;
	}
	
	return 0;
}

static void hello_exit(void)
{
	struct hellomsg *info=NULL;
	int i=0;

	printk("hello_exit\n");
	for(i=0;i<10;i++){
		dequeue(&qlst, info);
		printk("dequeue %d, %d\n", info->id, queue_len(&qlst));
		kfree(info);
		info=NULL;
	}
	
	
	return;
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
