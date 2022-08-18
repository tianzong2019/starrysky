# make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
KDIR :=${HOME}/codeSpace/base_kernel
out :=  ${HOME}/codeSpace/distaghil/out

modname:=mdrv

ifneq  ($(KERNELRELEASE),)

obj-m:=${modname}.o
$(info "2nd")

else

PWD:=$(shell pwd)
all:
	$(info "1st")
	make -C $(KDIR) M=$(PWD) modules
	${CROSS_COMPILE}gcc -static -o main test.c
	cp -f *.ko main ${out}

.PHONY:clean
clean:
	rm -rf *.ko *.o *.symvers *.mod.c *.mod.o *.mod *.order .tmp_versions .${modname}* main ${out}/*
endif
