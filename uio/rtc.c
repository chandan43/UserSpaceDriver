#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/io.h>

#include "rtc.h"

#define handle_error(msg) \
	           do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main (int argc,char *argv[])
{
	int retval;

	/*Get permission to access rtc port Address from userspace*/
	retval = ioperm (ADDRESS_REG, 0x04, 1);
	if (retval < 0) {
		handle_error("ioperm");
	}

	/* To Acess Year: */
	outb(YEAR,ADDRESS_REG);
	printf("Year = %ld\n",inb(DATA_REG));
	/*Month*/
	outb(MONTH,ADDRESS_REG);
	printf("Month = %ld\n",inb(DATA_REG));
	/*Day*/
	outb(DAY,ADDRESS_REG);
	printf("DAY = %ld\n",inb(DATA_REG));
	return 0;
}

