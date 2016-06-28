#!/bin/sh
# shellcheck source=utilities-src/local.sh
. "$(dirname "$0")"/local
emacsclient -s "$GLISP_EMACSID" -e "(kill-emacs)" || true
# The following code assumes there is only one pid in the pidfile
# Yes pkill would make this easier
# shellcheck disable=SC2009
if ps -p "$(cat "$SERVER_PIDFILE")" -o comm= |grep emacs; then
    kill "$(cat "$SERVER_PIDFILE")"
fi
# shellcheck disable=SC2009
if ps -p "$(cat "$SERVER_PIDFILE")" -o comm= |grep emacs; then
    kill -9 "$(cat "$SERVER_PIDFILE")"
fi
rm "$SERVER_PIDFILE"
