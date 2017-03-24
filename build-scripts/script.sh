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
cd "geany-utilities"
tup varsed quickproject.lisp.in quickproject.lisp
cd "../build-scripts"
tup varsed install-quicklisp.sh.in install-quicklisp.sh
tup varsed install.sh.in install.sh
cd ".."
sh ./build-scripts/install-quicklisp.sh .; tar zcf quicklisp.tgz quicklisp; rm -rf quicklisp
cd "geany-utilities"
tar -zmxf ../quicklisp.tgz ; sh make-manifest.sh ../quicklisp/setup.lisp; tar zcf quicklisp.tgz quicklisp ; rm -rf quicklisp
sh make.sh
cd "../geany-plugin"
tup varsed local.h.in local.h
$CONFIG_CC $CONFIG_CFLAGS -Wall -Wextra -Werror -fPIC $CONFIG_GEANY_CFLAGS -c -o completions.o completions.c
$CONFIG_CC $CONFIG_CFLAGS -Wall -Wextra -Werror -fPIC $CONFIG_GEANY_CFLAGS -c -o definitions.o definitions.c
$CONFIG_CC $CONFIG_CFLAGS -Wall -Wextra -Werror -fPIC $CONFIG_GEANY_CFLAGS -c -o indent.o indent.c
$CONFIG_CC $CONFIG_CFLAGS -Wall -Wextra -Werror -fPIC $CONFIG_GEANY_CFLAGS -c -o plugin-main.o plugin-main.c
$CONFIG_CC $CONFIG_CFLAGS -Wall -Wextra -Werror -fPIC $CONFIG_GEANY_CFLAGS -c -o project.o project.c
$CONFIG_CC $CONFIG_CFLAGS -Wall -Wextra -Werror -fPIC $CONFIG_GEANY_CFLAGS -c -o repl.o repl.c
$CONFIG_CC $CONFIG_CFLAGS -Wall -Wextra -Werror -fPIC $CONFIG_GEANY_CFLAGS -c -o server.o server.c
$CONFIG_CC $CONFIG_CFLAGS -Wall -Wextra -Werror -fPIC $CONFIG_GEANY_CFLAGS -c -o util.o util.c
$CONFIG_CC -shared $CONFIG_CFLAGS -Wall -Wextra -Werror -fPIC $CONFIG_GEANY_CFLAGS -o glisp.so completions.o definitions.o indent.o plugin-main.o project.o repl.o server.o util.o
cd "../test/utility-tests"
tup varsed setupenv.in setupenv
cd "../../utilities-src"
tup varsed filetypes.lisp.in filetypes.lisp
tup varsed glispinit.lisp.in glispinit.lisp
cd "../utilities"
cp ../utilities-src/filetypes.lisp filetypes.lisp
cp ../utilities-src/glispinit.lisp glispinit.lisp
cp ../utilities-src/glispinit2.lisp glispinit2.lisp
cp ../utilities-src/quicklisp.lisp quicklisp.lisp
cp ../utilities-src/slime-server.el slime-server.el
cp ../geany-utilities/geany-utilities geany-utilities
cp ../utilities-src/repl.glade repl.glade
cp ../utilities-src/repl2.glade repl2.glade
cd "$ROOT_DIR" || exit 1
tar xpf build-scripts/git-ignore.tar
