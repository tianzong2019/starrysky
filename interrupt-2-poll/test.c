#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>

/*
 * ./keyTest /dev/my_key0
 *
 */
int main(int argc, char **argv)
{
	int fd;
	int val;
	struct pollfd fds[1];
	int timeout_ms = 5000;
	int ret;

	fd = open("/dev/miscdev",O_RDWR);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}

	fds[0].fd = fd;
	fds[0].events = POLLIN;
	

	while (1)
	{
		/* int poll(struct pollfd *fds, nfds_t nfds, int timeout);
		 * 功能：监视并等待多个文件描述符的属性变化
		 * - fds：指向一个结构体数组的第0个元素的指针
		 *        struct pollfd{
					  int fd;			//文件描述符
					  short events;	//等待的事件
					  short revents;	//实际发生的事件
			      };
		 * - nfds：用来指定第一个参数数组元素个数
		 * - timeout：指定等待的毫秒数，无论 I/O 是否准备好，poll() 都会返回.
		 */
		ret = poll(fds, 1, timeout_ms);
		if ((ret == 1) && (fds[0].revents & POLLIN))
		{
			read(fd, &val, 1);
			printf("get button : 0x%x\r\n", val);
		}
		else
		{
			printf("timeout\r\n");
		}
	}
	close(fd);
	
	return 0;
}
