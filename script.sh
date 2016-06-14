#! /bin/sh -e
set -x
. ./tup.config
escapeSlashes() {
    printf "%s" "$1" | sed 's/\//\\\//g'
}

varsed() {
    (
    INSTALL_DIR="$(escapeSlashes "$CONFIG_INSTALL_DIR")"
    GEANY_PLUGIN_DIR="$(escapeSlashes "$CONFIG_GEANY_PLUGIN_DIR")"
    CFLAGS="$(escapeSlashes "$CONFIG_CFLAGS")"
    sed "s/@INSTALL_DIR@/$INSTALL_DIR/g
         s/@GEANY_PLUGIN_DIR@/$GEANY_PLUGIN_DIR/g
         s/@CFLAGS@/$CFLAGS/g" "$1" > "$2"
         )
     }
cd 'geany-plugin'
varsed local.h.in local.h
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -c -o completions.o completions.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -c -o definitions.o definitions.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -c -o indent.o indent.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -c -o plugin-main.o plugin-main.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -c -o server.o server.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -c -o util.o util.c
gcc $CONFIG_CFLAGS -Wall -Wextra -Werror -shared -fPIC `pkg-config --cflags geany` -o glisp.so completions.o definitions.o indent.o plugin-main.o server.o util.o
cd '..'
varsed install.sh.in install.sh


cat <<'EOF' > .gitignore
##### TUP GITIGNORE #####
##### Lines below automatically generated by Tup.
##### Do not edit.
.tup
/.gitignore
/install.sh
EOF


cat <<'EOF' > geany-plugin/.gitignore
##### TUP GITIGNORE #####
##### Lines below automatically generated by Tup.
##### Do not edit.
/.gitignore
/completions.o
/definitions.o
/glisp.so
/indent.o
/local.h
/plugin-main.o
/server.o
/util.o
EOF
