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
	reflex --decoration=none -r '\.(c|cpp|h)$$' make build

watch-test:
	reflex --decoration=none -r '\.(c|cpp|h)$$' -- sh -c 'echo "build start"; make build-test; echo "Watches established"'
