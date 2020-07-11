obj-m += cremona.o
cremona-y :=  main.o driver_device_manager.o driver_device.o driver_toot.o mpack.o serializer.o device_manager.o device.o toot.o communicate.o
cremona-y += interfaces/allocator.o interfaces/communicator.o interfaces/communicator_factory.o interfaces/id_mapper.o interfaces/id_mapper_factory.o interfaces/locker.o interfaces/locker_factory.o interfaces/waiter.o interfaces/waiter_factory.o interfaces/device_file.o interfaces/device_file_factory.o

ccflags-y := -DMPACK_HAS_CONFIG -std=gnu99 -Wno-declaration-after-statement

KDIR ?= /lib/modules/$(shell uname -r)/build
BUILD_DIR ?= $(CURDIR)/../../build/driver/module
BUILD_DIR_MAKEFILE ?= $(CURDIR)/../../build/driver/module/Makefile

default: $(BUILD_DIR_MAKEFILE)
	make -C $(KDIR) M=$(BUILD_DIR) src=$(CURDIR) modules

$(BUILD_DIR):
	mkdir -p "$@"
	mkdir -p "$@/interfaces"

$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	touch "$@"

clean:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(CURDIR) clean
