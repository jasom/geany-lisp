set -e

export glispTMPDIR;
#Using a consistent path for quicklisp means the ASDF cache can work
#We can't have quicklisp in-tree as tup wants a fully-specified output
glispTMPDIR="/tmp/glisptmp-$(echo "$PWD"|sha1sum|cut -d' ' -f1)"

mkdir -p "$glispTMPDIR"

if which tup; then
    tup
else
    echo "Tup not installed, falling back on manual build" >&2
    sh build-scripts/script.sh
fi

if ! grep -q '^tup.config$' .gitignore; then
    echo tup.config > .gitignore2
    echo /utilities/quicklisp >> .gitignore2
    cat .gitignore >> .gitignore2
    mv -f .gitignore2 .gitignore
fi
