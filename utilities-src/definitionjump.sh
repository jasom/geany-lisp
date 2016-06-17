#!/bin/sh
UNESCAPE="$(dirname "$0")/string.awk"
escape() {
    printf '%s\n' "$1"|sed 's/[\\"]/\\&/g'
}
SYMBOL="$(escape "$1")"
PACKAGE="$(escape "$2")"
ITEMS="$(emacsclient -s "$GLISP_EMACSID" -e "(ss-find-definitions \"$SYMBOL\" \"$PACKAGE\")"| awk -f "$UNESCAPE" |tr '\n' ' ')"
#printf '%s\n' "$ITEMS"
#printf '%q\n' "$ITEMS"
eval "for item in $ITEMS; do echo \$item; done"
