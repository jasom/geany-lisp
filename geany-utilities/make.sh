#!/bin/sh

set -e

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
     (with-open-file (f "quicklisp/manifest.txt")
     (loop for line = (read-line f nil nil)
     while line
     do 
         (asdf:load-asd (uiop:merge-pathnames* line (truename "quicklisp/"))))))
         ;unless (ignore-errors (asdf:load-asd (uiop:merge-pathnames* line "quicklisp/"))) do (setf errors t))))
(asdf:load-asd "geany-utilities.asd")
(asdf:load-system :geany-utilities)
(setf uiop/image:*image-entry-point* 'geany-utilities::entry-point)
;(trace geany-utilities::emacs-eval)
(uiop/image:dump-image "geany-utilities" :executable t)
EOF

