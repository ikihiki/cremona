ROOT := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
OUTDIR := $(ROOT)/build
DRIVER := $(OUTDIR)/driver/cremona.ko
DRIVER_SRC := $(wildcard $(ROOT)/driver/src/*.c) $(wildcard $(ROOT)/driver/src/*.h)
DAEMON := $(OUTDIR)/daemon/daemon
DAEMON_SRC_ROOT := $(ROOT)/daemon
DAEMON_SRC := $(wildcard $(DAEMON_SRC_ROOT)/cmd/daemon/**/*.go)
DAEMON_INTERNAL_SRC := $(wildcard $(DAEMON_SRC_ROOT)/internal/**/*.go)

$(DRIVER): $(DRIVER_SRC)
	make -C driver/src
$(DAEMON): $(DAEMON_INTERNAL_SRC) $(DAEMON_SRC)
	cd daemon/cmd/daemon; \
	go build -o $(DAEMON)


build: $(DRIVER) $(DAEMON);

clean:
	rm -r $(OUTDIR)

load: $(DRIVER)
	sudo insmod $(DRIVER)

unload: 
	sudo rmmod $(DRIVER)

agent-test: build
	$(DAEMON) --agent --dir /home/ibuki

