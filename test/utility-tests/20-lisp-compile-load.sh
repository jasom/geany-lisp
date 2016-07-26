. ./setupenv
set -e

cat - <<EOF > test-project/test.lisp
(declaim (optimize (debug 3)))
(in-package :cl-user)
(defun test-function ()
3)
EOF

"$GLISP_UTILITY_PATH/geany-utilities" lisp-compile-load "$PWD/test-project/test.lisp"
