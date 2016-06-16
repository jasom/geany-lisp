#!/bin/sh
# This is the header that is prepended to the generated build script
set -xe

sed "s/=/='/;s/$/'/" tup.config > tup_vars.sh

ROOT_DIR="$PWD"

CONFIG_VARS="$(grep -v '^#' tup.config|sed 's/=.*//')"
VARSED_EXPR="$(for item in $CONFIG_VARS; do
        echo "s/@${item#CONFIG_}@/\${${item#CONFIG_}}/g"
        done)"

if test -f geany-plugin/glisp.so; then
    echo "System appears to be built, exiting" >&2
    exit 0
fi
. ./tup_vars.sh
escapeSlashes() {
    printf "%s" "$1" | sed 's/\//\\\//g'
}

varsed() {
    (
    for item in $CONFIG_VARS; do
        eval "${item#CONFIG_}=\$(escapeSlashes \"\$$item\")"
    done
    sed "$(eval "printf '%s' \"$VARSED_EXPR\"")" "$1" > "$2"
         )
     }

alias tup='true &&' #this makes "tup varsed" act like "true && varsed"

# remainder of generated build script goes here
