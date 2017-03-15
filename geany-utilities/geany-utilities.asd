;;;; geany-utilities.asd

(asdf:defsystem #:geany-utilities
  :description "Describe geany-utilities here"
  :author "Your Name <your.name@example.com>"
  :license "Specify license here"
  :depends-on #.(with-open-file (f "deps.txt") (read f))
  :serial t
  :entry-point "geany-utilities::entry-point"
  :components ((:file "package")
               (:file "geany-utilities")
	       (:file "launch-lisp")
	       (:file "stop-lisp")
	       (:file "definition-jump")
	       (:file "lisp-complete")
	       (:file "lisp-build-project")
	       (:file "lisp-compile-load")
	       (:file "quickproject")
               (:file "repl")
	       (:file "lisp-indent")))
               

