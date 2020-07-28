#!/bin/bash

assert() {
    in=$1
    ans=$2
    echo "input: $1, ans: $2"
    cargo run -- $1 > out/asm.S

    riscv64-unknown-elf-gcc out/asm.S -o out/run
    qemu-riscv64 out/run

    res=$?
    
    if [ $res == $ans ]; then
        echo "OK!  Result: $res, Ans: $ans"
    else
        echo -e "\033[0;31mNG\033[0;39m"
        echo "FAIL! Result: $res, Ans: $ans"
    fi
}


cd $(cd $(dirname ${BASH_SOURCE:-$0}); pwd)
mkdir -p out

assert 0 0
assert 10 10
assert 1+3 4
assert 1+3+8 12
assert ' 1 + 2 + 8  ' 11
assert 2-3 255
assert 1-5+1 253
assert 1*3+4 7
assert 3+2*4-1 10
assert 4/2+2*4/1 10
assert 1+2*3*4 25
assert "3*(2+4)" 18
assert "1+((2)+4/2)" 5
assert "(3+4*2)+3*(1+2)-4" 16
cd -
