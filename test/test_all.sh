#!/bin/bash

target=interpreter
while getopts "n:s" arg; do
    case $arg in
        n)
            step=$OPTARG
            ;;
        s)
            target=codegen
            ;;
        ?)
            echo "usage: ./run_all.sh [-n <step>] [-s]"
            exit 0
            ;;
    esac
done
shift $((OPTIND -1))

[ -z $step ] && step=12

base_dir=$(dirname "$0")
ret=0

CC="riscv64-unknown-elf-gcc -march=rv32im -mabi=ilp32"
if [ $(uname) == "Linux" ]; then
    EMU=qemu-riscv32
else
    EMU="spike --isa=RV32G /usr/local/bin/pk"
fi

npm --prefix "$base_dir/.." run build

if [ "$target" == "codegen" ]; then
    # run codegen using minidecaf-tests/check.sh
    cd $base_dir/minidecaf-tests
    PROJ_PATH=../.. STEP_UNTIL=$step ./check.sh
else
    # run interpreter
    for ((i = 1; i <= $step; i++ )); do
        for file in "$base_dir"/minidecaf-tests/testcases/step$i/*.c; do
            $CC "$file"                     # compile with gcc
            $EMU ./a.out > /dev/null        # run it
            expected=$?                     # get exit code
            rm a.out

            echo "$file:"
            actual=`"$base_dir/run.sh" "$file" "$target"`

            if [ "$expected" == "$actual" ]; then
                echo "OK"
            else
                echo "expected: $expected"
                echo "actual:   $actual"
                echo "FAIL"
                ret=1
            fi
        done
    done
    exit $ret
fi
