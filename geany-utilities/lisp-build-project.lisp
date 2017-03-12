(in-package #:geany-utilities)

(defun lisp-build-project (asd-file-path system-name)
  (let ((raw-result
          (emacs-eval
            (format nil "(ss-eval-expr ~/gu:str/)"
                    (let ((*print-readably* t))
                      (prin1-to-string
                        `(progn
                           (asdf:load-asd ,asd-file-path)
                           (asdf:load-system ,system-name))))))))
    (write-sequence raw-result *error-output*)))
