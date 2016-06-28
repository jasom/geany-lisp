#!/bin/sh
# shellcheck source=utilities-src/local.sh
. "$(dirname "$0")"/local
PROJECT_ROOT="$(escape "$1")"
LISP_CMD="$(escape "$2")"
if "${EMACSCLIENT_BIN}" -s "$GLISP_EMACSID" -e t; then
    echo "Server already running" >&2
fi
mkdir -p "$(dirname "$SERVER_PIDFILE")"
"${EMACS_BIN}" -Q --daemon="${GLISP_EMACSID}" --load "${UTILITIES}/slime-server.el"
"${EMACSCLIENT_BIN}" -s "$GLISP_EMACSID" -e '(emacs-pid)' > "$SERVER_PIDFILE"
"${EMACSCLIENT_BIN}" -s "$GLISP_EMACSID" -e "(ss-start-server \"$PROJECT_ROOT/quicklisp/slime-helper.el\" \"$LISP_CMD\" \"$PROJECT_ROOT/quicklisp/glispinit.lisp\")"
