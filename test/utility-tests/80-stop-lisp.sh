
. "$(dirname "$0")/setupenv"

set -e

"$GLISP_UTILITY_PATH/stop-lisp"
! pgrep -F "/tmp/$(whoami)/glisp/emacs-glisp-test-id.pid" emacs
