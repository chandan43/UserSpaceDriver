#include  <stdlib.h>
#include  <stdio.h>
#include  <unistd.h>
#include  <fcntl.h>
#include  <sys/io.h>
#include  <signal.h>
#include  <sys/mman.h>

#define handle_error(msg) \
	           do { perror(msg); exit(EXIT_FAILURE); } while (0)


int main()
{
	pid_t cpid;
	void *res;
	int fd, ret, i;
	char *devmem, *devmem_start;
	struct sigaction act;
	uint nint;

	//if ((fd = open("/dev/uio0", O_RDONLY)) < 0) {
	if ((fd = open("/dev/uio0",O_RDWR)) < 0) {
		perror("Failed to open /dev/uio0\n");
		exit(EXIT_FAILURE);
	}

	cpid = fork();
	if (cpid == 0) {
		while (read(fd, &nint, sizeof(nint)) >= 0)
			printf("%s:Interrupts: %d\n", __func__, nint);
	} else {

		act.sa_flags = SA_NOCLDWAIT;
		act.sa_handler = SIG_DFL;

		if (sigaction(SIGCHLD, &act, NULL) == -1) ;
		perror("sigaction: ");

		getchar();
		devmem_start =
		    (char *)mmap(NULL,4096, PROT_READ | PROT_WRITE |PROT_EXEC,
				 MAP_SHARED, fd, 0);
		if (devmem_start == MAP_FAILED)
			 handle_error("mmap");

		printf("%s:device mapped @ %p\n", __func__, devmem_start);
		getchar();
		munmap(devmem_start, 4096);
		kill(cpid, SIGKILL);
	}

	return 0;
}
