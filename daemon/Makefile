INTERNAL_ROOT = ./internal
INTERNAL_DIR     := $(shell find $(INTERNAL_ROOT) -type d)
INTERNALS      =  $(foreach dir, $(INTERNAL_DIR), $(wildcard $(dir)/*.go))

DAEMON_ROOT = ./cmd/daemon
DAEMON_DIR     := $(shell find $(DAEMON_ROOT) -type d)
DAEMONS      =  $(foreach dir, $(DAEMON_DIR), $(wildcard $(dir)/*.go))
DAEMON_TARGET := ../build/daemon/cremona

WEB_ROOT = ./cmd/web
WEB_DIR     := $(shell find $(WEB_ROOT) -type d)
WEBS      =  $(foreach dir, $(WEB_DIR), $(wildcard $(dir)/*.go))
WEB_TARGET := ../build/daemon/web

$(DAEMON_TARGET): $(BUILD_DIR) $(INTERNALS) $(DAEMONS)
	 go build -o $(DAEMON_TARGET) $(DAEMON_ROOT)/main.go

$(WEB_TARGET): $(BUILD_DIR) $(INTERNALS) $(WEBS)
	go build -o $(WEB_TARGET) $(WEB_ROOT)/main.go

$(BUILD_DIR):
	mkdir -p "$@"

all: clean build

clean-daemon:
	go clean
	rm -f $(DAEMON_TARGET)

clean-web:
	go clean
	rm -f $(WEB_TARGET)

clean: clean-daemon clean-web

build-daemon: $(DAEMON_TARGET)

gen-swagger:
	cd $(WEB_ROOT) && ~/go/bin/swag i
build-web: gen-swagger $(WEB_TARGET)

build: build-daemon build-web

run-web: build-web
	$(WEB_TARGET)


watch:
	reflex --decoration=none -r '\.go$$' make build

watch-web:
	reflex --decoration=none -G 'cmd/web/docs/*' -r '\.go$$' -s make run-web