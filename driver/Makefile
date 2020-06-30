build-driver:
	make -C src

build-test:
	make -C test

build: build-driver build-test;

clean:
	make clean -C src
	make clean -C test

run-test:
	make run -C test

watch:
	export LC_ALL=C && export LANG=C; \
	make clean; \
	while true; do \
		echo build start; \
		make build-driver; \
		make build-test; \
		inotifywait -r -e modify ./; \
	done