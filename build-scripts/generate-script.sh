tup
tar cpf build-scripts/git-ignore.tar $(find . -iname .gitignore)
git clean -fX
tup generate --config generate.config script.sh
cat build-scripts/script-header.sh script.sh build-scripts/script-footer.sh> build-scripts/script.sh
rm script.sh
