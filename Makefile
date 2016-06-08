all: tup.config
	sh build.sh

tup.config:
	sh config.sh

install: all
	sh install.sh

