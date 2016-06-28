#!/bin/sh
# shellcheck source=utilities-src/local.sh
. "$(dirname "$0")"/local
# shellcheck disable=SC2119
emacsclient -s "$GLISP_EMACSID" -e "(ss-indent-lisp-string \"$(escape)\")"|unescape
