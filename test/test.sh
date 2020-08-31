make
assert() {
    ans=$1
    echo $2 > case.c
    riscv64-unknown-elf-gcc -march=rv32imac -mabi=ilp32 -S case.c
    ./main.out case.c output.s
    riscv64-unknown-elf-gcc -march=rv32imac -mabi=ilp32 output.s -o run    
    qemu-riscv32 run
    res=$?

    if [ $res = $ans ]; then
        echo "OK!  Result: $res, Ans: $ans"
    else
        echo -e "\033[0;31mNG\033[0;39m"
        echo "FAIL! Result: $res, Ans: $ans"
        exit 1
    fi
}

assert 0 'int main() { return 0; }'
assert 42 'int main() { return 42; }'
assert 0 'int main() { return !1; }'
assert 1 'int main() { return !0; }'
assert 254 'int main() { return ~1; }'
assert 255 'int main() { return ~0; }'
assert 10 'int main() { return - -10; }'
assert 0 'int main() { return -0; }'
assert 10 'int main() { return - -+10; }'
assert 15 'int main() { return 3*5; }'
assert 2 'int main() { return 8/4; }'
assert 1 'int main() { return 9%4; }'
assert 21 'int main() { return 5+20-4; }'
assert 41 'int main() { return  12 + 34 - 5 ; }'
assert 47 'int main() { return 5+6*7; }'
assert 15 'int main() { return 5*(9-6); }'
assert 4 'int main() { return (3+5)/2; }'
assert 10 'int main() { return -10+20; }'
