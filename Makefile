all: build utilities/quicklisp

build: tup.config
	sh build.sh

tup.config:
	sh config.sh

install: all
	sh install.sh

utilities/quicklisp: install-quicklisp.sh build
	sh install-quicklisp.sh
