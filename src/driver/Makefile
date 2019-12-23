obj-m += cremona.o
cremona-objs := main.o link.o mpack.o comm.o device.o toot_data_list.o

ccflags-y := -DMPACK_HAS_CONFIG -std=gnu99 -Wno-declaration-after-statement

KDIR ?= /lib/modules/$(shell uname -r)/build
BUILD_DIR ?= $(PWD)/../../build/driver
BUILD_DIR_MAKEFILE ?= $(PWD)/../../build/driver/Makefile

default: $(BUILD_DIR_MAKEFILE)
	make -C $(KDIR) M=$(BUILD_DIR) src=$(PWD) modules

$(BUILD_DIR):
	mkdir -p "$@"

$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	touch "$@"

clean:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(PWD) clean
