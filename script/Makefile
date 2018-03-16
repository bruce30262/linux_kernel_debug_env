# Makefile for kernel test
# ref: https://wwssllabcd.github.io/blog/2012/11/13/how-to-make-linux-module/
PWD         := $(shell pwd) 
# current version: 4.13.0-32-generic
KVERSION    := $(shell uname -r)
KERNEL_DIR   = /usr/src/linux-headers-$(KVERSION)/

# change MODULE_NAME for different kernel module name
MODULE_NAME  = test
obj-m       := $(MODULE_NAME).o   

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
