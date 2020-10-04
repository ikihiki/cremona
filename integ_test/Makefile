ROOT := $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))../)
OUTDIR := $(ROOT)/build
DRIVER := $(OUTDIR)/driver/cremonamod.ko
DRIVER_SRC := $(wildcard $(ROOT)/driver/src/*.c) $(wildcard $(ROOT)/driver/src/*.h)
DAEMON := $(OUTDIR)/daemon/daemon
DAEMON_SRC_ROOT := $(ROOT)/daemon
DAEMON_SRC := $(wildcard $(DAEMON_SRC_ROOT)/cmd/daemon/**/*.go)
DAEMON_INTERNAL_SRC := $(wildcard $(DAEMON_SRC_ROOT)/internal/**/*.go)

GET_STATUS_SRC := $(wildcard $(DAEMON_SRC_ROOT)/tools/get-stats/*.go)
GET_STATS := $(OUTDIR)/daemon/get_stats

DEVICE_OP_SRC := $(wildcard $(DAEMON_SRC_ROOT)/tools/device-op/*.go)
DEVICE_OP := $(OUTDIR)/daemon/device-op

TOOT_OC_SRC := $(wildcard $(DAEMON_SRC_ROOT)/tools/toot-oc/*.go)
TOOT_OC := $(OUTDIR)/daemon/toot-oc

SEND_TXT_DIR := $(DAEMON_SRC_ROOT)/tools/send-txt
SEND_TXT_SRC := $(wildcard $(SEND_TXT_DIR)/*.go)
SEND_TXT := $(OUTDIR)/daemon/send-txt


$(DRIVER): $(DRIVER_SRC)
	make -C ../driver/src
$(DAEMON): $(DAEMON_INTERNAL_SRC) $(DAEMON_SRC)
	cd ../daemon/cmd/daemon; \
	go build -o $(DAEMON)

$(DEVICE_OP) : $(DAEMON_INTERNAL_SRC) $(DEVICE_OP_SRC)
	cd ../daemon/tools/device-op; \
	go build -o $(DEVICE_OP)


$(TOOT_OC) : $(DAEMON_INTERNAL_SRC) $(TOOT_OC_SRC)
	cd ../daemon/tools/toot-oc; \
	go build -o $(TOOT_OC)

$(SEND_TXT) : $(DAEMON_INTERNAL_SRC) $(SEND_TXT_SRC)
	cd $(SEND_TXT_DIR); \
	go build -o $(SEND_TXT)


build: $(DRIVER) $(DEVICE_OP) $(TOOT_OC) $(SEND_TXT);

load: $(DRIVER)
	sudo insmod $(DRIVER)

unload: 
	sudo rmmod $(DRIVER)

device-test:
	make load
	$(DEVICE_OP)
	make unload

toot-test:
	make load
	$(TOOT_OC)
	make unload

text-test:
	make load
	$(SEND_TXT)
	make unload

clean:
	rm -r $(OUTDIR)