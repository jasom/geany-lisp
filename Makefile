all: run-tup utilities/quicklisp

run-tup: tup.config
	sh build-scripts/build.sh

tup.config:
	sh build-scripts/config.sh

install: all
	sh build-scripts/install.sh

build-scripts/install-quicklisp.sh: run-tup

utilities/quicklisp: build-scripts/install-quicklisp.sh
	sh build-scripts/install-quicklisp.sh

config:
	sh build-scripts/config.sh

build-script:
	sh build-scripts/generate-script.sh

clean:
	git clean -fdX
