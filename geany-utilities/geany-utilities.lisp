;;;; geany-utilities.lisp

(in-package #:geany-utilities)

;;; "geany-utilities" goes here. Hacks and glory await!
(defvar *emacs-id*)
(defvar *emacs-bin*)
(defvar *emacsclient-bin*)
(defvar *server-pidfile*)
(defvar *utility-path*)

(defun emacs-eval (string)
  (flexi-streams:with-output-to-sequence (outs)
    (run-program
     `(,*emacsclient-bin* "-s" ,*emacs-id* "-e" ,string)
     :error-output t
     :output outs
     :element-type '(unsigned-byte 8))))


;(trace uiop:getenvp)
(trace call-with-environment)
(trace entry-point)

(defun call-with-environment (function args)
  (let* ((*emacs-id* (getenvp "GLISP_EMACSID"))
	 (*emacs-bin* (or (getenvp "EMACS") "emacs"))
	 (*emacsclient-bin* (or (getenvp "EMACSCLIENT_BIN") "emacsclient"))
	 (*server-pidfile* (or (getenvp "SERVER_PIDFILE") (subpathname* (xdg-data-home) (format nil "glisp/~a.pid" *emacs-id*))))
	 (*utility-path*
	  (ensure-absolute-pathname
	   (ensure-directory-pathname (getenvp "GLISP_UTILITY_PATH"))
	   *default-pathname-defaults*)))
    (apply function args)))

(defparameter +valid-commands+ '(launch-lisp definition-jump lisp-indent lisp-compile-load
				 quickproject launch-lisp stop-lisp lisp-complete
                                 lisp-build-project))

(defun entry-point ()
  (handler-case
      (handler-bind ((t (lambda (x)
			  (dissect::present x *error-output*))))
	(let ((command (intern (string-upcase (car *command-line-arguments*))
			       :geany-utilities))
	      (args (cdr *command-line-arguments*)))
	  (if (member command +valid-commands+)
	      (call-with-environment command args)
	      (error "No command Found ~S" command))))
    (t () (uiop:quit 1))))

(defun unescape-byte-stream (in out)
  (assert (= (read-byte in) 34))
  (loop for byte = (read-byte in)
     while (not (eql byte 'eof))
     do (case byte
	  (92 ; \
	   (ecase (read-byte in)
	     (110 ; n
	      (write-byte 10 out))
	     (116 ; t
	      (write-byte 9 out))
	     (34 ; "
	      (write-byte 34 out))
	     (92 ; \
	      (write-byte 92 out))))
	  (34 (return))
	  (t (write-byte byte out)))))

(defun unescape-bytes (bytes out)
  (flex:with-input-from-sequence (in bytes)
    (unescape-byte-stream in out)))

(defun split-string-list (string &optional terminator)
  (flex:with-input-from-sequence (in string)
    (let ((in (flex:make-flexi-stream in)))
      (loop for result =
	   (handler-case
	       (flex:with-output-to-sequence (out)
		 (loop for byte = (read-byte in)
		    if (eql byte terminator) do (error 'end-of-file)
		    while (member byte '(10 9 32))
		    finally (flex:unread-byte byte in))
		 (case (flex:peek-byte in)
		   (34 ; #\"
		    (unescape-byte-stream in out))
		   (t
		    (loop for byte = (read-byte in nil nil)
		       when (member byte `(10 9 32 nil ,terminator))
			 do (if byte (flex:unread-byte byte in))
			    (return)
		       do (write-byte byte out)))))
	     (end-of-file () nil))
	 while result
	 collect result))))

(defun escape-stream (in out)
  (loop for char = (read-byte in nil 'eof)
     while (not (eql char 'eof))
     do
       (case char
	 ((#.(char-code #\\) #.(char-code #\"))
	  (write-char #\\ out)
	  (write-char (code-char char) out))
	 (t
	  (if (< char 128)
	      (write-char (code-char char) out)
	      (format out "\\~X\\ " char))))))

(defun escape-string (string out)
  (with-input-from-string (in string)
    (loop for char = (read-char in nil 'eof)
       while (not (eql char 'eof))
       do
	 (case char
	   ((#\\ #\")
	    (write-char #\\ out)
	    (write-char char out))
	   (t (write-char char out))))))

(defun esc (outs arg colon at)
  (declare (ignore at))
  (if colon
      (escape-stream arg outs)
      (escape-string arg outs)))

(defun str (outs arg colon at)
  (write-char #\" outs)
  (esc outs arg colon at)
  (write-char #\" outs))

