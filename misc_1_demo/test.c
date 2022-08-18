#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char const *argv[])
{
	int fd = 0;
	int val = 12;

	printf("%s %d\n", __func__, __LINE__);
	fd = open("/dev/miscdev", O_RDWR);

	write(fd, &val, sizeof(val));
	read(fd, &val, sizeof(val));
	printf("%s %d, val %d\n", __func__, __LINE__, val);

	close(fd);

	return 0;
}
