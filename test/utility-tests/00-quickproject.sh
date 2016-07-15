. "$(dirname "$0")/setupenv"

if test -z "$GLISP_RUN_SLOW_TESTS"; then
    tar xf test-project.tar
else
    "$GLISP_UTILITY_PATH/quickproject" ./test-project testproject
fi
