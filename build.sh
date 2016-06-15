set -e
if ! test -e tup.config; then
    sh config.sh
fi

if which tup; then
    tup
else
    echo "Tup not installed, falling back on manual build" >&2
    sh script.sh
fi

if ! grep -q '^tup.config$' .gitignore; then
    echo tup.config > .gitignore2
    cat .gitignore >> .gitignore2
    mv -f .gitignore2 .gitignore
fi
