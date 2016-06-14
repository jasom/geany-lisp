#!/bin/sh
ID="${GLISP_EMACSID:-glisp-$PPID}"
UNESCAPE="$(dirname $0)/string.awk"
EXPR='(ss-eval-expr
"(let ((outs (make-array 0 :element-type '\''character :fill-pointer 0 :adjustable t)))
  (with-output-to-string (s outs)
  (let ((*error-output* s)
        (*standard-output* s))
    (multiple-value-bind (truename warnings-p failure-p)
        (compile-file \"'"$1"'\")
        (unless failure-p
          (load truename))
          (finish-output s)
          (values (not failure-p) outs)))))")'
#echo "$EXPR" >&2
EXPR2="
(let* ((sldb-hook (lambda () (sldb-abort)))
       (results nil)
       buf
       (tag (cl-gensym))
       (slime-compilation-finished-hook (lambda (&rest r) (throw tag t))))
  (print \"$1\" t)
  (find-file \"$1\")
  (setq buf (get-file-buffer \"$1\"))
  (print (buffer-name buf) t)
  (unwind-protect
    (progn
     (catch tag
       (goto-char (point-min))
        (slime-compile-and-load-file)
        (while t
          (unless (eq (process-status (slime-connection)) 'open)
            (error \"Lisp connection closed unexpectedly\"))
          (accept-process-output nil 0.01)))
      (set-buffer buf)
      (while (slime-find-next-note)
        (push (list (line-number-at-pos) (get-char-property (point) 'help-echo)) results))
      (apply 'concat (mapcar (lambda (x) (format \"$1\\t%s\\t%s\\n\" (car x) (cadr x))) (reverse results))))
    (kill-buffer buf)))"


         
OUTPUT="$(emacsclient -s "$ID" -e "$EXPR2" | awk -f "$UNESCAPE")" 
printf "%s\n" "$OUTPUT" >&2

#printf "%s" $OUTPUT|grep -q '^(("T"'

