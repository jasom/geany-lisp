#Find our path
set -e
UTILITIES="$(cd "$(dirname "$0")" && pwd)"
test -d "$UTILITIES" || exit 1

# Varaibles for talking to the server
GLISP_EMACSID="${GLISP_EMACSID:-glisp-$PPID}"
EMACS_BIN="${EMACS:-emacs}"
EMACSCLIENT_BIN="${EMACSCLIENT_BIN:-emacsclient}"
SERVER_PIDFILE="/tmp/$(whoami)/glisp/emacs-$GLISP_EMACSID.pid"

# Tools for munging strings for emacs
UNESCAPE_AWK="$UTILITIES/string.awk"
myecho() {
    printf '%s\n' "$*"
}
alias echo=myecho
# shellcheck disable=SC2120
escape() {
    if test "$#" = 0; then
        sed 's/[\\"]/\\&/g'
    else
        #shellcheck disable=SC2119
        echo "$*"|escape
    fi
}
# shellcheck disable=SC2120
unescape() {
    if test "$#" = 0; then
        awk -f "$UNESCAPE_AWK"
    else
        #shellcheck disable=SC2119
        echo "$*"|unescape
    fi
}

UNESCAPE_ITEMS="no" # default is to not unescape items in list
# This splits a list of the form "foo bar" "baz "bif"
splitStringList() {
    if test "$UNESCAPE_ITEMS" = "yes"; then
        eval 'for item in '"$1"'; do echo "$(unescape \""$item"\")"; done'
    else
        eval "for item in $1; do echo \"\$item\"; done"
    fi
}
