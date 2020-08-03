#!/bin/bash
cat <<EOF | riscv64-unknown-elf-gcc -xc -c -o tmp2.o -
int ret3() { return 3; }
int ret5() { return 5; }
int add(int x, int y) { return x+y; }
int sub(int x, int y) { return x-y; }

int add6(int a, int b, int c, int d, int e, int f) {
  return a+b+c+d+e+f;
}
EOF


assert() {
    in=$1
    ans=$2
    echo "ans: $2,input: $1"
    ../src/main <(echo $in) >out/asm.S #change YOUR Compiler to build asm.S

    riscv64-unknown-elf-gcc out/asm.S tmp2.o -o out/run
    qemu-riscv64 out/run

    res=$?

    if [ $res == $ans ]; then
        echo "OK!  Result: $res, Ans: $ans"
    else
        echo -e "\033[0;31mNG\033[0;39m"
        echo "FAIL! Result: $res, Ans: $ans"
        exit 1
    fi
}

assert "0;" 0
assert "10;" 10
assert "1+3;" 4
assert "1+3+8;" 12
assert " 1 + 2 + 8  ;" 11
assert "2-3;" 255
assert "1-5+1;" 253
assert "1*3+4;" 7
assert "3+2*4-1;" 10
assert "4/2+2*4/1;" 10
assert "1+2*3*4;" 25
assert "3*(2+4);" 18
assert "1+((2)+4/2);" 5
assert "(3+4*2)+3*(1+2)-4;" 16
assert "-3;" 253
assert "-3+5;" 2
assert "100+-3*-3;" 109
assert "-2*+8;" 240
assert "4==4;" 1
assert "4==2;" 0
assert "4!=4;" 0
assert "4!=2;" 1
assert "4<5;" 1
assert "4<4;" 0
assert "4<=4;" 1
assert "4<3;" 0
assert "4>4;" 0
assert "4>3;" 1
assert "4>=5;" 0
assert "4>=4;" 1
assert "4+ 4*2 >=15+3*(-2);" 1
assert "a=3; b=a+1; b;" 4
assert "a=3; b=1+a*a; b;" 10
assert "a=3*2; b=2; c=2+b*a+10+2*b;" 28
assert "abc=3*2; bcde=2; c=2+bcde*abc;" 14
assert "abc=3*2; xy=6; abc==xy;" 1

assert "a=0; if (1) a=a+1; a;" 1
assert "a=0; if (0) a=a+1; a;" 0
assert "a=0; if (1) a=a+1; else a=a+2; a;" 1
assert "a=0; if (0) a=a+1; else a=a+2; a;" 2
assert "a=0; if (1) {a=a+1; a=a+2;} else {a=a+4; a=a+6;} a;" 3
assert "i=0; a=0; while(i<10){i=i+1;a=a+2;} a;" 20
assert "f() { 1; } f() + 2;" 3
assert "f() { a=1; a; } a=0; f(); a=a+2; a;" 2
assert "f(x) { x+1; } f(10);" 11
assert "f(x,y,z) { x+y+z; } f(1,2,3);" 6
assert "f(x) { x+1; } f(f(1));" 3
assert "f(x) { x=x+1; x; } x=1; y=f(x); x+y; }" 3

echo OK
