
. ./setupenv

trap 'rm -f definition-jump.out' EXIT
"$GLISP_UTILITY_PATH/definitionjump" "test-function" "CL-USER" > definition-jump.out
diff definition-jump.out outputs/definition-jump.out
