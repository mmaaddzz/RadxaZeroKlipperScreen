obj-m += st7796.o

ifndef LINUX_BUILD_DIR
	LINUX_BUILD_DIR := /lib/modules/$(shell uname -r)/build/
endif

default:
	make -C $(LINUX_BUILD_DIR) M=$(PWD) modules

install:
	sudo cp *.ko /lib/modules/$(shell uname -r)/kernel/drivers/gpu/drm/tiny
	sudo depmod -a

#modules_install:
#	make -C $(LINUX_BUILD_DIR) M=$(PWD) modules_install

clean:
	make -C $(LINUX_BUILD_DIR) M=$(PWD) clean
