(in-package #:geany-utilities)

(defun launch-lisp (project-root lisp-exec)
  (let ((lisp-init-file
	 (namestring
	  (uiop:merge-pathnames* 
	   (uiop:parse-unix-namestring "quicklisp/glispinit.lisp")
	   (ensure-directory-pathname project-root))))
	(slime-source
	 (namestring
	  (uiop:merge-pathnames* 
	   (uiop:parse-unix-namestring "quicklisp/slime-helper.el")
	   (ensure-directory-pathname project-root)))))
    (ensure-directories-exist
     (pathname-directory-pathname *server-pidfile*))

    (run-program `(,*emacs-bin*
		   "-Q"
		   ,(format nil "--daemon=~a" *emacs-id*)
		   "--load"  ,(unix-namestring
			       (subpathname* *utility-path* "slime-server.el"))))

    (let ((pid (emacs-eval "(emacs-pid)")))
      (with-open-file (f *server-pidfile*
			 :if-does-not-exist :create
			 :direction :output
			 :if-exists :supersede
			 :element-type '(unsigned-byte 8))
	(write-sequence pid f)))
    (emacs-eval (format nil "(ss-start-server ~/gu:str/ ~/gu:str/ ~/gu:str/)" slime-source lisp-exec lisp-init-file))

    (loop repeat 60
       when (ignore-errors (emacs-eval "(slime-eval t)") t) do (return)
	 do (sleep 1)
       finally (error "Unable to launch slime"))
    (sleep 1)))
