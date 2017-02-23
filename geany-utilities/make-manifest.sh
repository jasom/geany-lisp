#!/bin/sh

set -e

run_lisp () {
    sbcl --no-sysinit --no-userinit
}
QUICKLISP="./quicklisp/setup.lisp"

rm -f geany-utilities.manifest

run_lisp <<EOF >&2
#-asdf(require 'asdf)
(sb-ext:restrict-compiler-policy 'debug 3)
(asdf:initialize-source-registry
 '(:source-registry :ignore-inherited-configuration))
(load "$QUICKLISP")
(map 'nil 'ql:quickload (with-open-file (f "deps.txt") (read f)))
(ql:write-asdf-manifest-file t)
(uiop:quit)
EOF
