#! /bin/sh -e
cd 'utilities-src'
tup varsed quickproject.sh.in quickproject.sh
cd '../utilities'
cp ../utilities-src/definitionjump.sh definitionjump ; chmod +x definitionjump
cp ../utilities-src/launch-lisp.sh launch-lisp ; chmod +x launch-lisp
cp ../utilities-src/lispcompileload.sh lispcompileload ; chmod +x lispcompileload
cp ../utilities-src/lispcomplete.sh lispcomplete ; chmod +x lispcomplete
cp ../utilities-src/lispindent.sh lispindent ; chmod +x lispindent
cp ../utilities-src/positiontolinenumber.sh positiontolinenumber ; chmod +x positiontolinenumber
cp ../utilities-src/quickproject.sh quickproject ; chmod +x quickproject
cp ../utilities-src/stop-lisp.sh stop-lisp ; chmod +x stop-lisp
cd '../geany-plugin'
tup varsed local.h.in local.h
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -c -o completions.o completions.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -c -o definitions.o definitions.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -c -o indent.o indent.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -c -o plugin-main.o plugin-main.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -c -o project.o project.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -c -o server.o server.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -c -o util.o util.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -o glisp.so completions.o definitions.o indent.o plugin-main.o project.o server.o util.o
cd '../build-scripts'
tup varsed install-quicklisp.sh.in install-quicklisp.sh
tup varsed install.sh.in install.sh
