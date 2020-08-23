#!/bin/bash

assert() {
    ans=$1
    in=$2
    cargo run -q $2 > asm.S

    riscv64-unknown-elf-gcc -march=rv32imac -mabi=ilp32 asm.S -o out
    qemu-riscv32 out

    res=$?
    
    if [ $res == $ans ]; then
        echo "OK"
    else
        echo "Result: $res, Ans: $ans"
    fi
}


cd $(cd $(dirname ${BASH_SOURCE:-$0}); pwd)
rm asm.S out 2>/dev/null
assert 0 0
assert 10 10
rm asm.S out 2>/dev/null

