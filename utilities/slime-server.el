
(setq-default indent-tabs-mode nil)

(setq ss-bootstrap-string
"
(defun ssh-simple-eval (string &optional (package-name \"CL-USER\"))
  (let ((results
	 (multiple-value-list
	  (ignore-errors
	    (let ((*package* (find-package package-name)))
	      (multiple-value-list
	       (eval (read-from-string string))))))))
    (list (mapcar #'prin1-to-string (car results))
	  (and (cdr results)
	  (prin1-to-string (cadr results))))))
")

(defun ss-finish-setup ()
  (slime-eval
   `(cl:ignore-errors
     (cl:let ((cl:*package* (cl:find-package "CL-USER")))
       (cl:eval
	(cl:read-from-string ,ss-bootstrap-string))))))

(setq slime-lisp-implementations
      '(
	(sbcl ("sbcl") :init-function ss-finish-setup)))

(defun ss-setup-buffer ()
  (erase-buffer)
  (command-execute 'lisp-mode))

(defun ss-indent-lisp-string (str)
  (let ((x (get-buffer-create " myscratch.lisp")))
    (with-current-buffer x
      (ss-setup-buffer)
      (set (make-local-variable 'lisp-indent-function)
	   'common-lisp-indent-function)
      (set (make-local-variable 'indent-line-function)
	   'lisp-indent-line)
      (insert str)
      (goto-char (point-max))
      (lisp-indent-line)
      (buffer-string))))

(defun ss-complete-lisp-symbol (str)
  (let ((x (get-buffer-create " myscratch.lisp")))
    (with-current-buffer x
      (ss-setup-buffer)
      (set (make-local-variable 'lisp-indent-function)
	   'common-lisp-indent-function)
      (set (make-local-variable 'indent-line-function)
	   'lisp-indent-line)
      (insert str)
      (goto-char (point-max))
      (let* ((end (move-marker (make-marker) (slime-symbol-end-pos)))
	     (beg (move-marker (make-marker) (slime-symbol-start-pos)))
	     (completions (slime-contextual-completions beg end)))
	`(,(- (point-max) (marker-position beg))
	   ,(cl-second completions)
	   ,@(cl-first completions))))))


;;returns either ((&rest results) nil) or (nil error)
(defun ss-eval-expr (str)
  (let ((x (get-buffer-create " myscratch.lisp")))
    (with-current-buffer x
      (ss-setup-buffer)
      (slime-eval `(cl-user::ssh-simple-eval ,str)))))

(defun ss-find-definitions (name package)
  (let* ((slime-buffer-package package)
        (dfns (slime-find-definitions name))
        (result nil))
    (apply 'concat
           (mapcar (lambda (x) (format "%S\n" x))
                   (when dfns
                     (dolist (elt dfns (reverse result))
                       (let ((location (cdr (assoc :location (cdr elt)))))
                         (push (car elt) result)
                         (push (cadr (assoc :file location)) result)
                         (push (cadr (assoc :position location)) result))))))))

;; This is more involved in slime's internals than I would like
(defun ss-async-wait (fn args callback)
  (let* ((tag (cl-gensym "ss-async-wait")))
    (apply
      callback
      (catch
        tag
        (apply fn `(,@args ,(lambda (&rest args) (throw tag args))))
        (let ((inhibit-quit nil)
              (conn (slime-connection)))
          (while t
                 (unless (eq (process-status conn) 'open)
                   (error "Lisp connection closed unexpectedly"))
                 (accept-process-output nil 0.01)))))))

;(defun ss-uses-xrefs (symbol)

(defun ss-start-server (slime-source lisp-exec)
  (load (expand-file-name slime-source))
  (slime lisp-exec))
