#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>


int main(int argc, char **argv)
{
	int fd;
	int val;
	int timeout_ms = 5000;
	int ret;
	struct epoll_event event = {0};
	int epfd = 0;
	struct epoll_event ev;

	fd = open("/dev/miscdev",O_RDWR);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}

	/* nfd = epoll_creat(max_size);
	 * - epoll_create()创建一个epoll实例。
	 * - nfd为epoll句柄，
	 * - 参数max_size标识这个监听的数目最大有多大
	 */
	epfd = epoll_create(1);
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLHUP | EPOLLMSG | EPOLLPRI | EPOLLET;

	/* int epoll_ctl(int epfd , int op , int fd , struct epoll_event * event );
	 * - epfd：就是指定epoll文件描述符。
	 * - op : 需要执行的操作，添加，修改，删除，详细如下
	 *        EPOLL_CTL_ADD 在epoll的监视列表中添加一个文件描述符（即参数fd），指定监视的事件类型（参数event）。
　　 *    　　EPOLL_CTL_MOD 修改监视列表中已经存在的描述符（即参数fd），修改其监视的事件类型（参数event）。
　　 *    　　EPOLL_CTL_DEL 将某监视列表中已经存在的描述符（即参数fd）删除，参数event传NULL。
	 * - fd：需要添加，修改，删除的套接字
	 * - event：需要epoll监视的时间类型
	 */
	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

	while (1)
	{
		/* int epoll_wait(int epid, struct epoll_event *events, int maxevents, int timeout);
		 * 功能说明 ：阻塞一段时间并等待事件发生，返回事件集合，也就是获取内核的事件通知。
		 *            说白了就是遍历双向链表，把双向链表里的节点数据拷贝出来，拷贝完毕后就从双向链表移除。
		 * 参数
		 * - epid：epoll_create 返回的 epoll 对象描述符。
		 * - events：存放就绪的事件集合，这个是传出参数。
		 * - maxevents：代表可以存放的事件个数，也就是 events 数组的大小。
		 * - timeout：阻塞等待的时间长短，以毫秒为单位，如果传入 -1 代表阻塞等待。
		 *
		 * 返回值
		 * >0	代表有几个我们希望监听的事件发生了
		 * =0	timeout 超时时间到了
		 * <0	出错，可以通过 errno 值获取出错原因
		 */
		ret = epoll_wait(fd, &event, 1, timeout_ms);
		if (ret)
		{
			read(fd, &val, 1);
			printf("get button : 0x%x\r\n", val);
		}
	}
	close(fd);
	
	return 0;
}

#if 0
typedef union epoll_data {
　　　　　　void *ptr;
　　　　　  int fd;
　　　　　　uint32_t u32;
　　　　　　uint64_t u64;
} epoll_data_t;
 
struct epoll_event {
　　　　　　uint32_t events; /* Epoll 监视的事件类型 */
　　　　　　epoll_data_t data; /* 用户数据 */
};
#endif
