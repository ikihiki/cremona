obj-m += cremonamod.o
cremonamod-objs += cremona/dispatch.o cremona/device.o cremona/toot.o cremona/communicator.o cremona/mpack.o cremona/serializer.o module/device_store.o
cremonamod-objs += module/toot_store.o module/element_store.o module/store.o module/net_link_comm.o module/main.o 

ccflags-y := -DMPACK_HAS_CONFIG -std=gnu99 -Wno-declaration-after-statement

KDIR ?= /lib/modules/$(shell uname -r)/build
BUILD_DIR := $(abspath  $(CURDIR)/../../build/driver)
TARGET ?= $(BUILD_DIR)/cremonamod.ko

$(TARGET): $(BUILD_DIR)
	make -C $(KDIR) M=$(BUILD_DIR) src=$(CURDIR) modules

$(BUILD_DIR):
	mkdir -p "$@"
	mkdir -p "$@/cremona"
	mkdir -p "$@/module"
	touch "$@/Makefile"

build: $(TARGET)

all: clean build

clean:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(CURDIR) clean
	rm -r $(BUILD_DIR)

watch:
	reflex --decoration=none -r '\.(c|h)$$' make

test:
	echo $(BUILD_DIR)