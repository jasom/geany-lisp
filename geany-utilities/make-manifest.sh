#!/bin/sh

set -e

run_lisp () {
    sbcl --no-sysinit --no-userinit
}
QUICKLISP="${QUICKLISP:-$HOME/quicklisp/setup.lisp}"

rm -f geany-utilities.manifest

run_lisp <<EOF
#-asdf(require 'asdf)
(sb-ext:restrict-compiler-policy 'debug 3)
(asdf:initialize-source-registry
 '(:source-registry :ignore-inherited-configuration))
(load "$QUICKLISP")
(asdf:load-asd "geany-utilities.asd")
(ql:quickload "geany-utilities")
(ql:write-asdf-manifest-file "geany-utilities.manifest")
(uiop:quit)
EOF
