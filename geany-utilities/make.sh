#!/bin/sh

set -ex

glispTMPDIR=${glispTMPDIR:?}

trap 'rm -rf "$glispTMPDIR/quicklisp"' EXIT

tar -C "$glispTMPDIR" -zxf ./quicklisp.tgz

run_lisp () {
    sbcl --no-sysinit --no-userinit
}

run_lisp <<EOF
#-asdf(require 'asdf)
(sb-ext:restrict-compiler-policy 'debug 3)
(asdf:initialize-source-registry
 '(:source-registry :ignore-inherited-configuration))
(loop with errors = t
 repeat 10
 while errors
 do (setf errors nil)
     (with-open-file (f "$glispTMPDIR/quicklisp/manifest.txt")
     (loop for line = (read-line f nil nil)
     while line
     do 
         (asdf:load-asd (uiop:merge-pathnames* line (truename "$glispTMPDIR/quicklisp/"))))))
(asdf:load-asd (truename "geany-utilities.asd"))
(asdf:load-system :geany-utilities)
(setf uiop/image:*image-entry-point* 'geany-utilities::entry-point)
(uiop/image:dump-image "geany-utilities" :executable t)
EOF

