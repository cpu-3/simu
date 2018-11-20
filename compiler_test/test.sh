#! /bin/sh -x

COMPILER=../compiler
DIR=$COMPILER/test

$COMPILER/min-caml $DIR/$1
../asm/main.py $DIR/$1.s >& /dev/null
./simu a.out h
