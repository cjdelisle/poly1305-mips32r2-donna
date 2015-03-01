#!/bin/bash
die() { echo $1; exit 100; }
LOOPS=256
COMPILER="gcc -march=mips32r2 -O3 -fomit-frame-pointer -funroll-loops"

echo $COMPILER -I. auth.S bench.c -o measurepoly1305_asm
$COMPILER -I. auth.S bench.c -o measurepoly1305_asm || die "compile failed"

echo "testing..."
./measurepoly1305_asm 3 | grep '711396f175792e0426ede57ac433d2da' >/dev/null || die "test failed"
echo "passed"
echo
echo "measuring... ($LOOPS loops)"
time ./measurepoly1305_asm $LOOPS >/dev/null
