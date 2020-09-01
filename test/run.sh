#!/bin/bash

mkdir -p out/

CC="riscv64-unknown-elf-gcc -march=rv32im -mabi=ilp32"
if [ $(uname) = "Linux" ]; then
    EMU=qemu-riscv32
else
    EMU="spike --isa=RV32G /usr/local/bin/pk"
fi

base_dir=$(dirname "$0")
file_in=$(realpath "$1")
file_asm=$(realpath out/test.S)
file_exe=$(realpath out/run)
file_res=$(realpath out/test.res)
target=$2

if [ "$target" == "codegen" ]; then
    npm --prefix "$base_dir/.." run cli -- "$file_in" -s -o "$file_asm" > /dev/null
    $CC "$file_asm" -o "$file_exe"
    $EMU "$file_exe" > /dev/null
    res=$?
else
    npm --prefix "$base_dir/.." run cli -- "$file_in" -c -o "$file_res" > /dev/null
    res=$(cat "$file_res")
fi

rm -rf out/
echo $res
