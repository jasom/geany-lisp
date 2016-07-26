(in-package #:geany-utilities)

(defun lisp-compile-load (source-file)
  (let ((raw-result
	 (emacs-eval (format nil 
			     "(ss-compile-load ~/gu:str/)"
		    source-file))))
    (unescape-bytes raw-result *error-output*)))
