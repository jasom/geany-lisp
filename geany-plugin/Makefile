all: lisp-indent.so install

lisp-indent.so: lisp.c
	gcc -Wall -Wextra -o lisp-indent.so -shared -fPIC lisp.c $(shell pkg-config --cflags geany)

install:
	cp lisp-indent.so ~/.config/geany/plugins

clean:
	rm *.o *.so
