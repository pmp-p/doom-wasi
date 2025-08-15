#!/bin/bash
reset

[ -d xcc ] || git clone --recursive --no-tags --depth 1 --single-branch --branch main https://github.com/tyfkda/xcc

pushd xcc
    make clean
    make -j$(nproc) \
     CC=clang OPTIMIZE="-O0 -g3"  \
     xcc

    make -j$(nproc) \
     CC=clang OPTIMIZE="-O0 -g3"  \
     wcc
popd


XCC=$(pwd)/xcc

ulimit -c unlimited
rm -f /tmp/core-wcc.*
sudo sysctl -w kernel.core_pattern=/tmp/core-%e.%p.%h.%t

if make \
 -C doomgeneric -f Makefile.wasi \
 CC=${XCC}/wcc \
 AS=${XCC}/as \
 LD=${XCC}/ld \
 OUTPUT=doom.wcc \
 CFLAGS="-I${XCC}/include -DXCC -D__wasi__ -D__wasm32__ -DNORMALUNIX -DSNDSERV -D_DEFAULT_SOURCE" \
 LDFLAGS= \
 LIBS=""
then

    if which wasmtime
    then
        echo -n "found wasmtime :"
    else
        curl https://wasmtime.dev/install.sh -sSf | bash
        echo -n "installed wasmtime :"
    fi
    wasmtime --version
    WASMTIME_BACKTRACE_DETAILS=1 wasmtime --dir . ./doomgeneric/doom.wcc
else
    if file /tmp/core-wcc.*
    then
        pushd xcc
            gdb wcc -c /tmp/core-wcc.* <<END
bt
q
END
        popd
    fi

fi

