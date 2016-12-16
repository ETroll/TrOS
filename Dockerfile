FROM debian:jessie
MAINTAINER Karl Syvert Løland <karl.syvert@myerart.net>

# Set APT_GET_UPDATE to make consistent images
ENV APT_GET_UPDATE 2016-03-01
ENV code /src
ENV toolchain /tool/gcc-i386-none-elf

RUN DEBIAN_FRONTEND=noninteractive apt-get --quiet --yes update \
    && DEBIAN_FRONTEND=noninteractive apt-get --quiet --yes install \
        binutils \
        gcc \
        g++ \
        nasm \
        make \
        wget \
        libmpc-dev \
        dosfstools \
        mtools \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists

RUN wget http://ftp.gnu.org/gnu/binutils/binutils-2.24.tar.gz \
    && tar xvzf binutils-2.24.tar.gz \
    && cd binutils-2.24 \
    && ./configure --prefix=${toolchain} --target=i386-elf --disable-nls --disable-werror\
    && make \
    && make install \
    && cd .. \
    && rm -rf binutils-2.24 \
    && rm binutils-2.24.tar.gz

RUN wget http://ftp.gnu.org/gnu/gcc/gcc-4.9.0/gcc-4.9.0.tar.gz \
    && tar xvzf gcc-4.9.0.tar.gz \
    && cd gcc-4.9.0 \
    && ./configure --prefix=${toolchain} --target=i386-elf --disable-nls --enable-languages=c --without-headers --disable-werror\
    && make all-gcc\
    && make install-gcc\
    && cd .. \
    && rm -rf gcc-4.9.0 \
    && rm gcc-4.9.0.tar.gz

RUN DEBIAN_FRONTEND=noninteractive apt-get --quiet --yes remove \
    g++ \
    wget \
    libmpc-dev

# COPY . ${code}
WORKDIR ${code}
ENTRYPOINT ["make"]
