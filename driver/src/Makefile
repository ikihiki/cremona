obj-m += cremona.o
cremona-objs := main.o  mpack.o serializer.o device_manager.o device_map.o device.o util.o toot_map.o toot.o 

ccflags-y := -DMPACK_HAS_CONFIG -std=gnu99 -Wno-declaration-after-statement

KDIR ?= /lib/modules/$(shell uname -r)/build
BUILD_DIR ?= $(CURDIR)/../../build/driver
BUILD_DIR_MAKEFILE ?= $(CURDIR)/../../build/driver/Makefile

default: $(BUILD_DIR_MAKEFILE)
	make -C $(KDIR) M=$(BUILD_DIR) src=$(CURDIR) modules

$(BUILD_DIR):
	mkdir -p "$@"

$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	touch "$@"

clean:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(CURDIR) clean
