(in-package #:geany-utilities)

(defun repl-output (start-position)
  (declare (ignore start-position))
  (let ((raw-result
          (emacs-eval "(ss-output-text)")))
    (unescape-bytes raw-result *standard-output*)))

(defun repl-prompt ()
  (let ((raw-result
          (emacs-eval "(ss-prompt)")))
    (unescape-bytes raw-result *standard-output*)))

(defun repl-send-input (input offset)
  (emacs-eval (format nil "(ss-input-and-return ~/gu:str/ ~a)" input offset))
  (unescape-bytes
    (emacs-eval "(ss-input)") *standard-output*))
