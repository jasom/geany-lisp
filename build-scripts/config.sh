PREFIX="${PREFIX:-/usr/local}"
FAKE_PREFIX="${FAKE_PREFIX:-$PREFIX}"
cat > tup.config <<EOF
CONFIG_INSTALL_DIR=${INSTALL_DIR:-$PREFIX/share/glisp}
CONFIG_FAKE_INSTALL_DIR=${FAKE_INSTALL_DIR:-${INSTALL_DIR:-$FAKE_PREFIX/share/glisp}}
CONFIG_GEANY_PLUGIN_DIR=${GEANY_PLUGIN_DIR:-$HOME/.config/geany/plugins}
CONFIG_CFLAGS=${CFLAGS:--g3}
CONFIG_LISP_CMD=${LISP_CMD:=sbcl --no-sysinit --no-userinit}
CONFIG_CC=${CC:-cc}
CONFIG_GEANY_CFLAGS=$(pkg-config --cflags geany) --std=c99
EOF
