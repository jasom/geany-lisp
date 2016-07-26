
. "$(dirname "$0")/setupenv"

set -e

"$GLISP_UTILITY_PATH/geany-utilities" stop-lisp
! pgrep -f -- --daemon=glisp-test-id
