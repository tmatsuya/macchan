ifneq ($(KERNELRELEASE),)
obj-m		:= macchan.o
else
KDIR		:= /lib/modules/$(shell uname -r)/build/
KDIR2		:= /lib/modules/$(shell uname -r)/kernel/
PWD		:= $(shell pwd)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) V=1 modules
	gcc -fPIC -shared -o gettime.so gettime.c

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean 
	rm -f *.so

install:
	cp -p $(PWD)/*.ko $(KDIR2)/drivers/net/
	depmod -a
endif
