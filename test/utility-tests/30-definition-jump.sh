
. ./setupenv

trap 'rm -f definition-jump.out' EXIT
"$GLISP_UTILITY_PATH/geany-utilities" definition-jump "test-function" "CL-USER" > definition-jump.out
diff definition-jump.out outputs/definition-jump.out
