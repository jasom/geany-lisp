(in-package :geany-helper)

(defun simple-eval (string &optional (package-name "CL-USER"))
  (let ((results
	 (multiple-value-list
	  (ignore-errors
	    (let ((*package* (find-package package-name)))
	      (multiple-value-list
	       (eval (read-from-string string))))))))
    (list (mapcar #'prin1-to-string (car results))
	  (and (cdr results)
	  (prin1-to-string (cadr results))))))

(push (uiop:merge-pathnames* (uiop:parse-unix-namestring "lisp/")
                            *project-root*)
      ql:*local-project-directories*)
(ql:use-only-quicklisp-systems)
