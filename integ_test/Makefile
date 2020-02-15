
ROOT := $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))../)
OUTDIR := $(ROOT)/build
DRIVER := $(OUTDIR)/driver/cremona.ko
DRIVER_SRC := $(wildcard $(ROOT)/driver/src/*.c) $(wildcard $(ROOT)/driver/src/*.h)
DAEMON := $(OUTDIR)/daemon/daemon
DAEMON_SRC_ROOT := $(ROOT)/daemon
DAEMON_SRC := $(wildcard $(DAEMON_SRC_ROOT)/cmd/daemon/**/*.go)
DAEMON_INTERNAL_SRC := $(wildcard $(DAEMON_SRC_ROOT)/internal/**/*.go)
GET_STATUS_SRC := $(wildcard $(DAEMON_SRC_ROOT)/tools/get-stats/*.go)
GET_STATS := $(OUTDIR)/daemon/get_stats
$(DRIVER): $(DRIVER_SRC)
	make -C ../driver/src
$(DAEMON): $(DAEMON_INTERNAL_SRC) $(DAEMON_SRC)
	cd ../daemon/cmd/daemon; \
	go build -o $(DAEMON)
$(GET_STATS) : $(DAEMON_INTERNAL_SRC) $(GET_STATUS_SRC)
	cd ../daemon/tools/get-stats; \
	go build -o $(GET_STATS)

build: $(DRIVER) $(GET_STATS);

load: $(DRIVER)
	sudo insmod $(DRIVER)

unload: 
	sudo rmmod $(DRIVER)

load-test: 
	make load
	$(GET_STATS)
	make unload

test: 
	@echo $(DRIVER_SRC)
clean:
	rm -r $(OUTDIR)