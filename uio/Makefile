#obj-m := uioirq.o
#obj-m += uiommap.o
#obj-m += uioportio.o
obj-m += uioapic.o


KDIR= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	$(CC) uiotest.c -o uiotest
	$(CC) uiommaptest.c -o uiommaptest
	$(CC) uiopoll.c -o uiopoll
	$(CC) uioapictest.c -o uioapictest

clean: 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
	rm -rf uiotest
	rm -rf uiommaptest
	rm -rf uiopoll
