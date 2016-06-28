#!/bin/sh
# This is the header that is prepended to the generated build script
set -xe

sed "s/=/='/;s/$/'/" tup.config > tup_vars.sh

ROOT_DIR="$PWD"

CONFIG_VARS="$(grep -v '^#' tup.config|sed 's/=.*//')"
VARSED_EXPR="$(for item in $CONFIG_VARS; do
        echo "s/@${item#CONFIG_}@/\${${item#CONFIG_}}/g"
        done)"

if test -f geany-plugin/glisp.so; then
    echo "System appears to be built, exiting" >&2
    exit 0
fi
. ./tup_vars.sh
escapeSlashes() {
    printf "%s" "$1" | sed 's/\//\\\//g'
}

varsed() {
    (
    for item in $CONFIG_VARS; do
        eval "${item#CONFIG_}=\$(escapeSlashes \"\$$item\")"
    done
    sed "$(eval "printf '%s' \"$VARSED_EXPR\"")" "$1" > "$2"
         )
     }

alias tup='true &&' #this makes "tup varsed" act like "true && varsed"

# remainder of generated build script goes here
#! /bin/sh -e
cd 'utilities-src'
tup varsed glispinit.lisp.in glispinit.lisp
tup varsed quickproject.sh.in quickproject.sh
cd '../utilities'
cp ../utilities-src/definitionjump.sh definitionjump ; chmod +x definitionjump
cp ../utilities-src/launch-lisp.sh launch-lisp ; chmod +x launch-lisp
cp ../utilities-src/lispcompileload.sh lispcompileload ; chmod +x lispcompileload
cp ../utilities-src/lispcomplete.sh lispcomplete ; chmod +x lispcomplete
cp ../utilities-src/lispindent.sh lispindent ; chmod +x lispindent
cp ../utilities-src/local.sh local ; chmod +x local
cp ../utilities-src/quickproject.sh quickproject ; chmod +x quickproject
cp ../utilities-src/stop-lisp.sh stop-lisp ; chmod +x stop-lisp
cp ../utilities-src/glispinit.lisp glispinit.lisp
cp ../utilities-src/glispinit2.lisp glispinit2.lisp
cp ../utilities-src/quicklisp.lisp quicklisp.lisp
cp ../utilities-src/string.awk string.awk
cp ../utilities-src/slime-server.el slime-server.el
cd '../geany-plugin'
tup varsed local.h.in local.h
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC $(pkg-config --cflags geany) -c -o completions.o completions.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC $(pkg-config --cflags geany) -c -o definitions.o definitions.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC $(pkg-config --cflags geany) -c -o indent.o indent.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC $(pkg-config --cflags geany) -c -o plugin-main.o plugin-main.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC $(pkg-config --cflags geany) -c -o project.o project.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC $(pkg-config --cflags geany) -c -o server.o server.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC $(pkg-config --cflags geany) -c -o util.o util.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC $(pkg-config --cflags geany) -o glisp.so completions.o definitions.o indent.o plugin-main.o project.o server.o util.o
cd '../build-scripts'
tup varsed install-quicklisp.sh.in install-quicklisp.sh
tup varsed install.sh.in install.sh
cd "$ROOT_DIR" || exit 1
tar xpf build-scripts/git-ignore.tar
