#!/bin/sh
. "$(dirname "$0")"/local
OUTPUT="$(emacsclient -s "$GLISP_EMACSID" -e "(ss-complete-lisp-symbol \"$(escape)\")")"
OUTPUT="${OUTPUT%\)}"
OUTPUT="${OUTPUT#\(}"

UNESCAPE_ITEMS="yes" splitStringList "$OUTPUT"
