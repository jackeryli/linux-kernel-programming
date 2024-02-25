obj-m = jackdriver.o
PWD := $(shell pwd)
EXTRA_CFLAGS = -Wall -g
CROSS = aarch64-linux-gnu-
KDIR = /home/jack/Desktop/linux/

all: clean
	$(MAKE) ARCH=arm64 CROSS_COMPILE=$(CROSS) -C $(KDIR) M=$(PWD) modules
		aarch64-linux-gnu-gcc test_rw.c -o test_rw
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf test_rw