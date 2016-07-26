(in-package #:geany-utilities)

;; TODO check pid-file and kill
(defun stop-lisp ()
  (emacs-eval "(kill-emacs)"))
