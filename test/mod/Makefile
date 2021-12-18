obj-m = main.o

all:
	make -C /lib/modules/5.4.0-91-generic/build M=$(PWD) modules

clean:
	make -C /lib/modules/5.4.0-91-generic/build M=$(PWD) clean

bazel-build: all
	cp "$(PWD)/main.ko" "$(PREFIX)/"