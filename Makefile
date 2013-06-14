ifneq ($(KERNELRELEASE),)
obj-m		:= macchan.o
else
KDIR		:= /lib/modules/$(shell uname -r)/build/
PWD		:= $(shell pwd)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) V=1 modules
	gcc -fPIC -shared -o gettime.so gettime.c

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean 
	rm -f *.so

install:
	install -m 644 $(PWD)/*.ko /lib/modules/`uname -r`/kernel/drivers/net
	depmod -a
endif
