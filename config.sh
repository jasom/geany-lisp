PREFIX="${PREFIX:-/usr}"
cat > tup.config <<EOF
CONFIG_INSTALL_DIR=$PREFIX/share/glisp
CONFIG_GEANY_PLUGIN_DIR=${GEANY_PLUGIN_DIR:-$HOME/.config/geany/plugins}
EOF
