obj-m := uioirq.o
obj-m += uiommap.o


KDIR= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	$(CC) uiotest.c -o uiotest
	$(CC) uiommaptest.c -o uiommaptest
	$(CC) uiopoll.c -o uiopoll

clean: 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -rf uiotest
	rm -rf uiommaptest
	rm -rf uiopoll
