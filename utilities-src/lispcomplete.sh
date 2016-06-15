#!/bin/sh
INPUT="$(cat -|
sed 's/[\\"]/\\&/g')"
OUTPUT="$(emacsclient -s "$GLISP_EMACSID" -e '(ss-complete-lisp-symbol "'"$INPUT"'")' 
)"
OUTPUT="${OUTPUT%\)}"
OUTPUT="${OUTPUT#\(}"

#printf "%s\n" "$OUTPUT"
for item in $OUTPUT; do
    printf "%s\n" "$item"|
    sed 's/^"//;s/\([^\]\)"/\1/g;s/\\\(["\\]\)/\1/g';
done

