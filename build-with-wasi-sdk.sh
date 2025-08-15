#!/bin/bash
reset
if [ -f WASI_SDK_PREFIX/bin/clang ]
then
    echo wasi-sdk-*
else
    wget -O- -q https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-27/wasi-sdk-27.0-$(arch)-linux.tar.gz | tar xz
    mv $(pwd)/wasi-sdk-* WASI_SDK_PREFIX
fi


echo "

    ----------------------- building ------------------------

/Users/user/bin/wasmtime
found wasmtime :wasmtime 33.0.0 (4ce8232ab 2025-05-20)
Error: failed to compile: wasm[0]::function[358]::CheckBossEnd


"

CC_DIR=$(pwd)/WASI_SDK_PREFIX/bin

mkdir -p ./bin
rm -f bin/doom-wasisdk.wasm

if echo $@|grep clean
then
    make -C doomgeneric clean
fi

if make \
 -C doomgeneric -f Makefile.wasi \
 CC=${CC_DIR}/clang \
 AR=${CC_DIR}/ar \
 LD=${CC_DIR}/ld \
 OUTPUT=../bin/doom-wasisdk.wasm \
 CFLAGS="-O0 -g3 --target=wasm32-wasip1 -DNORMALUNIX -DSNDSERV -D_DEFAULT_SOURCE" \
 LDFLAGS= \
 LIBS=""
then
    mkdir -p ./bin
    mv ./doomgeneric/doom.wasi ./bin/doom-wasisdk.wasm
    if which wasmtime
    then
        echo -n "found wasmtime :"
    else
        curl https://wasmtime.dev/install.sh -sSf | bash
        . $HOME/.bashrc
        echo -n "installed wasmtime :"

    fi
    which wasmtime && wasmtime --version
    # WASMTIME_BACKTRACE_DETAILS=1 wasmtime --dir . ./bin/doom-wasisdk.wasm
    exit 0
else
    echo "

    build failed

"
        exit $LINENO

fi

