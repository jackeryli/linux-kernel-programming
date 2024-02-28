obj-m = myfs.o
PWD := $(shell pwd)
EXTRA_CFLAGS = -Wall -g
CROSS = aarch64-linux-gnu-
KDIR = /home/jack/Desktop/linux/

all: clean
	$(MAKE) ARCH=arm64 CROSS_COMPILE=$(CROSS) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean