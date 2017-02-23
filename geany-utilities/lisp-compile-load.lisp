(in-package #:geany-utilities)

(defun lisp-compile-load (source-file)
  (let ((raw-result
	 (emacs-eval (format nil 
			     "(ss-compile-load ~/gu:str/)"
		    source-file))))
    (unescape-bytes raw-result *error-output*)
    ;; output looks like ["nil...] for failing output
    (when (eql (elt raw-result 1) (char-code #\n))
      (uiop:quit 1))))
            
