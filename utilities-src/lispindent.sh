#!/bin/sh
. "$(dirname "$0")"/local
emacsclient -s "$GLISP_EMACSID" -e "(ss-indent-lisp-string \"$(escape)\")"|unescape
