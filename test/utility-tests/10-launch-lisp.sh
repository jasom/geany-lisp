. "$(dirname "$0")/setupenv"

set -e

"$GLISP_UTILITY_PATH/geany-utilities" launch-lisp test-project "$GLISP_INIT"
