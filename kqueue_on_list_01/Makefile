ARCH := arm64
ARMCC := aarch64-linux-gnu-
KDIR := ${HOME}/codeSpace/base_kernel
out :=  ${HOME}/codeSpace/distaghil/out

ifneq  ($(KERNELRELEASE),)

obj-m:=mdrv.o
$(info "2nd")

else

all: 
	$(info "1st")
	make -C $(KDIR) M=$(shell pwd) ARCH=$(ARCH) CROSS_COMPILE=$(ARMCC) modules
	# copy to outdir
	cp -f *.ko ${out}


.PHONY:clean
clean:
	rm -rf *.ko *.o *.symvers *.mod.c *.mod.o *.order .tmp_versions .*.cmd ${out}/*.ko
endif
