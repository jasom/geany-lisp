#!/bin/sh
# shellcheck source=utilities-src/local.sh
. "$(dirname "$0")"/local
# shellcheck disable=SC2119
OUTPUT="$(emacsclient -s "$GLISP_EMACSID" -e "(ss-complete-lisp-symbol \"$(escape)\")")"
OUTPUT="${OUTPUT%\)}"
OUTPUT="${OUTPUT#\(}"

UNESCAPE_ITEMS="yes" splitStringList "$OUTPUT"
