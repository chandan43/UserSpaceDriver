/*Uioint test application*/
#include  <stdlib.h>
#include  <stdio.h>
#include  <unistd.h>
#include  <fcntl.h>
#include  <sys/io.h>
#include <poll.h>

int main()
{
	int fd, ret;
	unsigned int nint;
	if ((fd = open("/dev/uio0", O_RDWR)) < 0) {
		perror("Failed to open /dev/uio0\n");
		exit(EXIT_FAILURE);
	}
	struct pollfd fds = {
            .fd = fd,
            .events = POLLIN,
        };
	ret = poll(&fds, 1, -1);
        if (ret >= 1) {
		while (read(fd, &nint, sizeof(nint)) >= 0)
			printf("%s:Interrupts: %d\n",__func__,  nint);
	}else {
            perror("poll()");
            close(fd);
            exit(EXIT_FAILURE);
        }
	close(fd);	
	return 0;
}
