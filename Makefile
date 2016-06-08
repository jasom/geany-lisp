all: tup.config
	sh build.sh

tup.config:
	sh config.sh

