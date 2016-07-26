
. ./setupenv

trap 'rm -f lisp-indent.out' EXIT
head -c -1 test-project/test.lisp |"$GLISP_UTILITY_PATH/geany-utilities" lisp-indent > lisp-indent.out
diff lisp-indent.out outputs/lisp-indent.out
