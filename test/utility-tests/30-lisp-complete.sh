

. ./setupenv

set -e
trap 'rm -f lisp-complete.out' EXIT
head -c -1 <<EOF | "$GLISP_UTILITY_PATH/geany-utilities" lisp-complete > lisp-complete.out
(cl-user::test-func
EOF
#cat lisp-complete.out>&2
diff lisp-complete.out outputs/lisp-complete.out
