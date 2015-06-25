#! /bin/sh

export PROJECT_HOME=~/Projects/Private/TrOS/

###
brew install gcc

cd $PROJECT_HOME
mkdir Toolchains
cd Toolchains
mkdir tmp
mkdir gcc-i386-none-elf

export CC=/usr/local/bin/gcc-4.9
export CXX=/usr/local/bin/g++-4.9
export LD=/usr/local/bin/gcc-4.9
export CFLAGS=-Wno-error

cd tmp
curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.24.tar.gz
curl -O http://ftp.gnu.org/gnu/gcc/gcc-4.9.0/gcc-4.9.0.tar.gz


#binutils
tar xvzf binutils-2.24.tar.gz
cd binutils-2.24

./configure --prefix=$PROJECT_HOME/Toolchains/gcc-i386-none-elf --target=i386-elf --disable-nls
make
make install

#gcc
cd ..
tar xvzf gcc-4.9.0.tar.gz
cd gcc-4.9.0

./configure --prefix=$PROJECT_HOME/Toolchains/gcc-i386-none-elf --target=i386-elf --disable-nls --enable-languages=c --without-headers
make all-gcc
make install-gcc

cd $PROJECT_HOME/Toolchains
rm -rf tmp/
