#!/bin/sh
# shellcheck source=utilities-src/local.sh
. "$(dirname "$0")"/local
SYMBOL="$(escape "$1")"
PACKAGE="$(escape "$2")"
# shellcheck disable=SC2119
ITEMS="$(emacsclient -s "$GLISP_EMACSID" -e "(ss-find-definitions \"$SYMBOL\" \"$PACKAGE\")"| unescape |tr '\n' ' ')"
#printf '%s\n' "$ITEMS"
#printf '%q\n' "$ITEMS"
splitStringList "$ITEMS"
