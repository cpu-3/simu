#! /bin/sh -x

COMPILER=../compiler
DIR=../cpuex2018/raytracer

$COMPILER/min-caml $DIR/minrt -inline 20
../asm/main.py $DIR/minrt.s >& /dev/null
./simu a.out < compiler_test/output.bin > dump.pbm
