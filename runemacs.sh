emacs -Q --daemon=slime-server --load slime-server.el
emacsclient -s slime-server -e '(emacs-pid)' > emacs.pid
