obj-m += uioirq.o


KDIR= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	$(CC) uiotest.c -o uiotest

clean: 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -rf uiotest
