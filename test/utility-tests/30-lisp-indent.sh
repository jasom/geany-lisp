
. ./setupenv

trap 'rm -f lisp-indent.out' EXIT
"$GLISP_UTILITY_PATH/lispindent" "test-function" "CL-USER" > lisp-indent.out < test-project/test.lisp
diff lisp-indent.out outputs/lisp-indent.out
