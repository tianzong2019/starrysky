
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
 
void sig_handler(int sig)
{
	if(sig == SIGIO)
	{
		printf("Receive io signal from kernel! %d\n", sig);
	}
}

int main(void)
{
	int fd;

	signal(SIGIO, sig_handler);

	fd = open("/dev/miscdev",O_RDWR);

	/* int fcntl(int fd, int cmd, long arg); 作用是改变一个已打开文件的属性
	 * cmd是命令罗列如下
	 * - F_SETOWN（设置异步IO所有权）
	 * - F_GETFL（获取文件flags）
	 * - F_SETFL（设置文件flags）
	 * -
	 * - F_DUPFD, F_GETFD, F_SETFD, F_SETLK, F_SETLKW, F_GETLK, F_GETOWN
	 *
	 * arg是要改变的属性内容
	 *
	 * 缺陷: 
	 * 当有多个文件发送异步通知信号给一个进程时，进程无法知道是哪个文件发送的信号，
	 * 这时候还是要借助poll的帮助完成IO
	 */
	fcntl(fd, F_SETOWN, getpid());
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | FASYNC);

	printf("waiting key interrupt:\n");

	while(1) {}
}
