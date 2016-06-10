set -e
if ! test -e tup.config; then
    sh config.sh
fi

tup

if ! grep -q '^tup.config$' .gitignore; then
    echo tup.config > .gitignore2
    cat .gitignore >> .gitignore2
    mv -f .gitignore2 .gitignore
fi
