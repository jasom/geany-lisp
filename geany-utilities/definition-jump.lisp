(in-package #:geany-utilities)

(defun definition-jump (symbol package)
  (let ((raw-result
	 (emacs-eval
	  (format nil "(ss-find-definitions ~/gu:str/ ~/gu:str/)"
		  symbol package))))
    (loop for item in
	 (split-string-list (flex:with-output-to-sequence (o)
			      (unescape-bytes raw-result o)))
	 do (map nil
		 (rcurry #'write-byte *standard-output*)
		 (nsubstitute 32 10 item))
	 (write-byte 10 *standard-output*))))
