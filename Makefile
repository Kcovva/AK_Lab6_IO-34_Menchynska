
# normal makefile
KDIR ?= /lib/modules/`uname -r`/build

DEST_ROOT ?= $(HOME)/repos/busybox/_install
KERNEL_VERSION := 4.19.325
INSTALL_DIR := $(DEST_ROOT)/lib/modules/$(KERNEL_VERSION)/extra

default:
	$(MAKE) -C $(KDIR) M=$$PWD
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
install: default
	mkdir -p $(INSTALL_DIR)
	find . -maxdepth 1 -name "*.ko" -exec cp {} $(INSTALL_DIR) \;
	@echo "module installed successfuly!"	