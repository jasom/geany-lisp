#!/bin/sh
UNESCAPE="$(dirname $0)/string.awk"
NL='
'

#set -x
INPUT="$(sed 's/["\\]/\\&/g')"
printf 'INPUT:\n%s\nENDINPUT' "$INPUT" > /tmp/lispindent.log
OUTPUT="$(emacsclient -s "$GLISP_EMACSID" -e '(ss-indent-lisp-string "'"${INPUT}"'")' |
awk -f "$UNESCAPE")"

printf 'OUTPUT:\n%s\nENDOUTPUT' "$OUTPUT" >> /tmp/lispindent.log
printf "%s\n" "$OUTPUT"
