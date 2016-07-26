(in-package #:geany-utilities)

(defun lisp-indent ()
  (unescape-bytes (emacs-eval
		   (format nil "(ss-indent-lisp-string ~:/gu:str/)" *standard-input*))
		  *standard-output*))
