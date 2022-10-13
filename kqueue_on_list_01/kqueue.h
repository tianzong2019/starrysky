#ifndef _KQUEUE_H_
#define _KQUEUE_H_
#include <linux/list.h>

struct queuelist {
	struct list_head head;
	int              max;
	int              cur;
};

#define queue_init(ql, m)\
do{\
	INIT_LIST_HEAD(&(ql)->head);\
	(ql)->max=(m);\
	(ql)->cur = 0;\
}while(0)

#define queue_len(ql)\
({\
	(ql)->cur;\
})

#define enqueue(q, n)\
({\
	int ret = 0;\
	if((q)->max <= (q)->cur) {\
		printk("queue full\n");\
		ret = -1;\
	} else {\
		list_add(&(n)->node, &(q)->head);\
		(q)->cur++;\
	}\
	ret;\
})

#define enqueue_tail(q, n)\
({\
	int ret = 0;\
	if((q)->max <= (q)->cur) {\
		printk("queue full\n");\
		ret = -1;\
	} else {\
		 list_add_tail(&(n)->node, &(q)->head);\
		(q)->cur++;\
	}\
	ret;\
})

#define dequeue(q, n)\
({\
	int ret=0;\
	if((q)->cur<=0) {\
		printk("queue empty\n");\
		ret=-1;\
	}else{\
		(q)->cur--;\
		typeof((q)->head.next) __qh_next =(q)->head.next;\
		n = (typeof(n))(__qh_next - (size_t) & (((typeof(n))0)->node));\
		 list_del(__qh_next);\
	}\
	ret;\
})

#define dequeue_tail(q, n)\
({\
	int ret=0;\
	if((q)->cur<=0) {\
		printk("queue empty\n");\
		ret=-1;\
	}else{\
		(q)->cur--;\
		typeof((q)->head) *__qh_prev =(q)->head.prev;\
		n = (typeof(n))(__qh_prev - (size_t) & (((typeof(n))0)->node));\
		 list_del(__qh_prev);\
	}\
	ret;\
})




#endif
