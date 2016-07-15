set -e
cd utility-tests
rm -f test.stdout test.stderr
for item in *.sh; do
    printf '%s ... ' "$(basename "$item")" >&2
    if sh "$item" >>test.stdout 2>>test.stderr; then
        printf 'Passed\n'
    else
        printf 'Failed\n'
        sh 99-cleanup.sh
        exit 1
    fi
done
