(load (expand-file-name "~/quicklisp/slime-helper.el"))

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

(command-execute 'slime)

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





