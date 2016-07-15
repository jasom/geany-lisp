. "$(dirname "$0")/setupenv"

set -e

"$GLISP_UTILITY_PATH/launch-lisp" test-project "$GLISP_INIT"
