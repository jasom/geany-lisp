#!/bin/sh
ID="$GLISP_EMACSID"
UTILITIES="$(dirname "$0")"
EMACS_BIN="${EMACS:-emacs}"
EMACSCLIENT_BIN="${EMACSCLIENT_BIN:-emacsclient}"
PIDFILE="/tmp/$(whoami)/glisp/emacs-${ID}.pid"
if "${EMACSCLIENT_BIN}" -s "$ID" -e t; then
    echo "Server already running" >&2
fi
mkdir -p "$(dirname "$PIDFILE")"
"${EMACS_BIN}" -Q --daemon="${ID}" --load "${UTILITIES}/slime-server.el"
"${EMACSCLIENT_BIN}" -s "$ID" -e '(emacs-pid)' > "$PIDFILE"
