INTERNAL_ROOT = ./internal
INTERNAL_DIR     := $(shell find $(INTERNAL_ROOT) -type d)
INTERNALS      =  $(foreach dir, $(INTERNAL_DIR), $(wildcard $(dir)/*.go))

DAEMON_ROOT = ./cmd/daemon
DAEMON_DIR     := $(shell find $(DAEMON_ROOT) -type d)
DAEMONS      =  $(foreach dir, $(DAEMON_DIR), $(wildcard $(dir)/*.go))
DAEMON_TARGET := ../build/daemon/cremona

$(DAEMON_TARGET): $(BUILD_DIR) $(INTERNALS) $(DAEMONS)
	 go build -o $(DAEMON_TARGET) $(DAEMON_ROOT)/main.go

$(BUILD_DIR):
	mkdir -p "$@"

all: clean build

clean-daemon:
	go clean
	rm -f $(DAEMON_TARGET)

clean: clean-daemon

build-daemon: $(DAEMON_TARGET)

build: build-daemon

watch:
	reflex --decoration=none -r '\.go$$' make build