(in-package #:geany-utilities)

(defun lisp-complete ()
  (let ((raw-result
	 (emacs-eval
	  (format nil "(ss-complete-lisp-symbol ~:/gu:str/)" *standard-input*))))
    (format *error-output* "~A~%" (map 'string #'code-char raw-result))
    (loop for item in (split-string-list (subseq raw-result 1 (1- (length raw-result)))
					 41)
	 do
	 (loop for byte across item
	      do (write-byte byte *standard-output*))
	 (terpri))))
