all: run-tup utilities/quicklisp

run-tup: tup.config
	sh build-scripts/build.sh

tup.config:
	sh build-scripts/config.sh

install: all
	sh build-scripts/install.sh

build-scripts/install-quicklisp.sh: run-tup

quicklisp.tgz: run-tup

utilities/quicklisp: quicklisp.tgz
	tar -C utilities -xf quicklisp.tgz

config:
	sh build-scripts/config.sh

build-script:
	sh build-scripts/generate-script.sh

clean:
	git clean -fdX

.PHONY:

test: .PHONY all
	cd test; sh run-tests.sh

test-full: .PHONY all
	cd test; GLISP_RUN_SLOW_TESTS=yes sh run-tests.sh
