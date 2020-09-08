#!/bin/bash
export CC="riscv64-unknown-elf-gcc  -march=rv32im -mabi=ilp32"
export QEMU=qemu-riscv32
USE_PARALLEL=${true:-$USE_PARALLEL}
JOBS=(testcases/step{1,2,3,4,5,6,7,8,9,10,11,12}/*.c)
make

run_job() {
    infile=$1
    outbase=${infile%.c}

    rm $outbase.{gcc,expected,err,my,actual,s} 1>/dev/null 2>&1

    $CC $infile -o $outbase.gcc
    $QEMU $outbase.gcc
    echo $? > $outbase.expected


    riscv64-unknown-elf-gcc -march=rv32imac -mabi=ilp32 -S $infile -o case.s
    ./main.out $infile output.s
    riscv64-unknown-elf-gcc -march=rv32imac -mabi=ilp32 output.s -o run    
    qemu-riscv32 run
    echo $? > $outbase.actual

    if ! diff -qZ $outbase.expected $outbase.actual >/dev/null ; then
        echo "FAIL ${infile}"
        echo "FAIL! Result: $ans, Ans: $expected"
        return 1
    else
        echo "OK ${infile}"
        return 0
    fi
}
export -f run_job


check_env() {
    if $CC --version >/dev/null 2>&1; then
        echo "gcc found"
    else
        echo "gcc not found"
        exit 1
    fi

    if $QEMU -version >/dev/null 2>&1; then
        echo "qemu found"
    else
        echo "qemu not found"
        exit 1
    fi
}


check_env;

for job in ${JOBS[@]}; do
    if ! run_job $job; then break; fi
done

