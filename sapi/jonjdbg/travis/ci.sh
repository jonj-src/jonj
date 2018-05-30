#!/usr/bin/env sh
git clone https://github.com/jonj/jonj-src
cd jonj-src
git checkout $JONJ
cd sapi
rm -rf jonjdbg
git clone https://github.com/krakjoe/jonjdbg.git
cd ../
./buildconf --force
./configure --disable-all --enable-jonjdbg --enable-maintainer-zts
make
