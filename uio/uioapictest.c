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
	char * Ioapic_start;
	struct sigaction act;
	uint nint;

	//if ((fd = open("/dev/uio0", O_RDONLY)) < 0) {
	if ((fd = open("/dev/uio0",O_RDWR)) < 0) {
		perror("Failed to open /dev/uio0\n");
		exit(EXIT_FAILURE);
	}

	Ioapic_start =
		    (char *)mmap(NULL,4096, PROT_READ | PROT_WRITE |PROT_EXEC,
				 MAP_SHARED, fd, 0);
	if (Ioapic_start == MAP_FAILED)
			 handle_error("mmap");

	printf("%s:device mapped @ %p\n", __func__, Ioapic_start);
	getchar();
	munmap(Ioapic_start, 4096);
	return 0;
}
