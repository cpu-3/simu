#! /bin/sh

TEST_DIR=compiler/test
for i in `find ../compiler/test -type f -mindepth 1 -maxdepth 1 -name "*.ml"`; do
    name=`basename $i .ml`
    if [ $name = "mandelbrot" ]; then
        :
    else
        echo compiler/test/$name
        ../compiler/min-caml ../$TEST_DIR/$name -inline 10 >& /dev/null
        ../asm/main.py ../$TEST_DIR/$name.s >& /dev/null
        ./simu a.out h
        echo '\n'
    fi
done
