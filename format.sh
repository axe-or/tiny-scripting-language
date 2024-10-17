set -xe

dos2unix *.c *.h
sed -E 's/\s*$//g' *.c *.h -i

VIM_CMD=':bufdo normal gg=GZZ'

nvim *.c *.h -c "$VIM_CMD"
