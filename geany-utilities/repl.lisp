(in-package #:geany-utilities)

(defun repl-output ()
  (loop
    (let ((raw-result
            (emacs-eval "(ss-output-text)")))
      (unless (= 3 (length raw-result))
        (format *error-output* "repl-done ~A~%" (length raw-result))
        (unescape-bytes raw-result *standard-output*)
        (return-from repl-output)))
    (sleep 0.25)))

(defun repl-prompt ()
  (let ((raw-result
          (emacs-eval "(ss-prompt)")))
    (unescape-bytes raw-result *standard-output*)))

(defun repl-send-input (input offset)
  (emacs-eval (format nil "(ss-input-and-return ~/gu:str/ ~a)" input offset))
  (unescape-bytes
    (emacs-eval "(ss-input)") *standard-output*))
