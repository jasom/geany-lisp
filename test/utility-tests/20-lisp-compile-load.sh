. ./setupenv
set -e

cat - <<EOF > test-project/test.lisp
(declaim (optimize (debug 3)))
(in-package :cl-user)
(defun test-function ()
3)
EOF

"$GLISP_UTILITY_PATH/lispcompileload" "$PWD/test-project/test.lisp"
