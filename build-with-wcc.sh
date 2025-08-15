#!/bin/bash
reset

if  [ -d xcc ]
then
    echo "local wip xcc"
else
    echo "xcc (patched)"
    git clone --recursive --no-tags --depth 1 --single-branch --branch main https://github.com/tyfkda/xcc

    pushd xcc
        wget https://patch-diff.githubusercontent.com/raw/tyfkda/xcc/pull/214.diff
        patch -p1 <  214.diff
    popd
fi

pushd xcc
    make clean >/dev/null 2>&1
    make -j$(nproc) \
     CC=clang OPTIMIZE="-O0 -g3"  \
     all wcc >/tmp/log 2>&1 || ( cat /tmp/log  && exit $LINENO )
popd


XCC=$(pwd)/xcc

ulimit -c unlimited
rm -f /tmp/core-wcc.*
sudo sysctl -w kernel.core_pattern=/tmp/core-%e.%p.%h.%t
echo "

    ----------------------- building ------------------------

"
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
    if ls /tmp/core-wcc.* 2>/dev/null
    then
        echo "

            ------------ crash ------------

"
        pushd xcc
            gdb wcc -c /tmp/core-wcc.* <<END
bt
q
END
        exit $LINENO
        popd
    else
        echo "

    build failed

"
        echo $LINENO
    fi

fi

