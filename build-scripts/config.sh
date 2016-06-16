PREFIX="${PREFIX:-/usr/local}"
cat > tup.config <<EOF
CONFIG_INSTALL_DIR=${INSTALL_DIR:-$PREFIX/share/glisp}
CONFIG_GEANY_PLUGIN_DIR=${GEANY_PLUGIN_DIR:-$HOME/.config/geany/plugins}
CONFIG_CFLAGS=${CFLAGS:--g3}
CONFIG_LISP_CMD=${LISP_CMD:=sbcl --no-sysinit --no-userinit}
EOF
