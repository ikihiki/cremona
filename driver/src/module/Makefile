obj-m += cremona.o
cremona-objs := ../cremona/dispatch.o ../cremona/device.o ../cremona/mpack.o ../cremona/serializer.o device_store.o toot_store.o element_store.o store.o main.o 

ccflags-y := -DMPACK_HAS_CONFIG -std=gnu99 -Wno-declaration-after-statement

KDIR ?= /lib/modules/$(shell uname -r)/build
BUILD_DIR ?= $(CURDIR)/../../../build/driver/module
TARGET ?= $(CURDIR)/../../../build/driver/module/cremona.ko

$(TARGET): $(BUILD_DIR)
	make -C $(KDIR) M=$(BUILD_DIR) src=$(CURDIR) modules

$(BUILD_DIR):
	mkdir -p "$@"

all: clean $(TARGET)

clean:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(CURDIR) clean

watch:
	reflex --decoration=none -r '\.(c|h)$$' make