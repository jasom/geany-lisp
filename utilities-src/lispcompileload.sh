#!/bin/sh
# shellcheck source=utilities-src/local.sh
. "$(dirname "$0")"/local
UNESCAPE="$(dirname "$0")/string.awk"

EXPR2="
(let* ((sldb-hook (lambda () (sldb-abort)))
       (results nil)
       buf
       (done nil)
       (slime-compilation-finished-hook (lambda (&rest r) (setf done t))))
  (print \"$1\" t)
  (find-file \"$1\")
  (setq buf (get-file-buffer \"$1\"))
  (print (buffer-name buf) t)
  (unwind-protect
    (progn
    (goto-char (point-min))
        (slime-compile-and-load-file)
        (while (not done)
          (unless (eq (process-status (slime-connection)) 'open)
            (error \"Lisp connection closed unexpectedly\"))
          (accept-process-output nil 0.01))
      (set-buffer buf)
      (while (slime-find-next-note)
        (push (list (line-number-at-pos) (get-char-property (point) 'help-echo)) results))
      (apply 'concat (mapcar (lambda (x) (format \"$1\\t%s\\t%s\\n\" (car x) (cadr x))) (reverse results))))
    (kill-buffer buf)))"

         
OUTPUT="$(emacsclient -s "$GLISP_EMACSID" -e "$EXPR2" | awk -f "$UNESCAPE")" 
printf "%s\n" "$OUTPUT" >&2

#printf "%s" $OUTPUT|grep -q '^(("T"'

