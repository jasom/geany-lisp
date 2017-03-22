set -e

export glispTMPDIR;
#Using a consistent path for quicklisp means the ASDF cache can work
#We can't have quicklisp in-tree as tup wants a fully-specified output
glispTMPDIR="/tmp/glisptmp-$(echo "$PWD"|sha1sum|cut -d' ' -f1)"

tup

rm -f build-scripts/git-ignore.tar
tar -cpf build-scripts/git-ignore.tar .gitignore
rm -rf utilities/quicklisp
find . -iname .gitignore -exec tar -rpf build-scripts/git-ignore.tar {} '+'
git clean -fX
tup generate --config generate.config script.sh
cat build-scripts/script-header.sh script.sh build-scripts/script-footer.sh> build-scripts/script.sh
rm script.sh
