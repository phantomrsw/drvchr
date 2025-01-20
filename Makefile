#CONFIG_MODULE_SIG=n

#obj-m += drvchr.o

ifeq ($(KERNEL_VERSION),)
	KERNEL_VERSION=$(shell uname -r)
endif

KDIR = /lib/modules/$(KERNEL_VERSION)/build
INSTALL_DIR =  /lib/modules/$(KERNEL_VERSION)/kernel/drivers
#CROSS_COMPILE = /bin/x86_64-linux-gnu-


MAKEARGS := -C $(KDIR)
MAKEARGS += $(if $(ARCH),ARCH=$(ARCH))
MAKEARGS += $(if $(CROSS_COMPILE),CROSS_COMPILE=$(CROSS_COMPILE))


$(info "MAKEARGS: $(MAKEARGS)")
$(info "INSTALL_DIR: $(INSTALL_DIR)")

.PHONY: default checkpatch clean


default:
	$(MAKE) $(MAKEARGS) M=$(PWD) modules


DRV_SOURCES = drvchr.c


checkpatch:
	cd $(KDIR) && (! $(KDIR)/scripts/checkpatch.pl -f --no-tree $(DRV_SOURCES:%=$(PWD)/%) | grep ERROR:)

clean:
	$(MAKE) $(MAKEARGS) M=$(PWD) clean
	rm test

install:
	cp $(PWD)/drvchr.ko $(INSTALL_DIR)
	depmod -a

load:
	rmmod drvchr
	insmod drvchr.ko

test:test.cpp
	g++ -std=c++20 -lm -o test test.cpp
