INTERNAL_ROOT = ./internal
INTERNAL_DIR     := $(shell find $(INTERNAL_ROOT) -type d)
INTERNALS      =  $(foreach dir, $(INTERNAL_DIR), $(wildcard $(dir)/*.go))

DAEMON_ROOT = ./cmd/daemon
DAEMON_DIR     := $(shell find $(DAEMON_ROOT) -type d)
DAEMONS      =  $(foreach dir, $(DAEMON_DIR), $(wildcard $(dir)/*.go))
DAEMON_TARGET := ../build/daemon/cremona

WEB_MASTER_ROOT  = ./cmd/web/master
WEB_MASTER_DIR   := $(shell find $(WEB_MASTER_ROOT) -type d)
WEB_MASTER_FILES =  $(foreach dir, $(WEB_MASTER_DIR), $(wildcard $(dir)/*.go))
WEB_MASTER_TARGET := ../build/daemon/web-master

WEB_AGENT_ROOT  = ./cmd/web/agent
WEB_AGENT_DIR   := $(shell find $(WEB_AGENT_ROOT) -type d)
WEB_AGENT_FILES =  $(foreach dir, $(WEB_AGENT_DIR), $(wildcard $(dir)/*.go))
WEB_AGENT_TARGET := ../build/daemon/web-agent

$(DAEMON_TARGET): $(BUILD_DIR) $(INTERNALS) $(DAEMONS)
	 go build -o $(DAEMON_TARGET) $(DAEMON_ROOT)/main.go

$(WEB_MASTER_TARGET): $(BUILD_DIR) $(INTERNALS) $(WEB_MASTER_FILES)
	go build -o $(WEB_MASTER_TARGET) $(WEB_MASTER_ROOT)/main.go

$(WEB_AGENT_TARGET): $(BUILD_DIR) $(INTERNALS) $(WEB_AGENT_FILES)
	go build -o $(WEB_AGENT_TARGET) $(WEB_AGENT_ROOT)/main.go 

$(BUILD_DIR):
	mkdir -p "$@"

all: clean build

clean-daemon:
	go clean
	rm -f $(DAEMON_TARGET)

clean-web:
	go clean
	rm -f $(WEB_MASTER_TARGET)
	rm -f $(WEB_AGENT_TARGET)

clean: clean-daemon clean-web

build-daemon: $(DAEMON_TARGET)

gen-swagger:
	cd $(WEB_MASTER_ROOT) && ~/go/bin/swag i
	cd $(WEB_AGENT_ROOT) && ~/go/bin/swag i
build-web: gen-swagger $(WEB_MASTER_TARGET) $(WEB_AGENT_TARGET)

build: build-daemon build-web

run-web: build-web
	$(WEB_MASTER_TARGET)


watch:
	reflex --decoration=none -r '\.go$$' make build

watch-web:
	reflex --decoration=none -G 'cmd/web/*/docs/*' -r '\.go$$' -s make run-web