obj-m += cremona.o
cremona-objs := main.o driver_device_manager.o driver_device.o driver_toot.o mpack.o serializer.o device_manager.o device_map.o device.o util.o toot_map.o toot.o 

ccflags-y := -DMPACK_HAS_CONFIG -std=gnu99 -Wno-declaration-after-statement

KDIR ?= /lib/modules/$(shell uname -r)/build
BUILD_DIR ?= $(CURDIR)/../../build/driver
TARGET ?= $(CURDIR)/../../build/driver/cremona.ko

$(TARGET): $(BUILD_DIR)
	make -C $(KDIR) M=$(BUILD_DIR) src=$(CURDIR) modules

$(BUILD_DIR):
	mkdir -p "$@"

all: clean $(TARGET)

clean:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(CURDIR) clean

watch:
	reflex --decoration=none -r '\.(c|h)$$' make