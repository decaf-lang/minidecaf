## setup developing envrionment
```
## install rust tools
## install riscv-64 gcc tools which can also compile&build riscv-32 apps.
## install qemu-risc32 simulator for RISC-V 32 User Apps
```
## download/build minidecaf
```
$ git clone https://github.com/decaf-lang/minidecaf.git
$ cd minidecaf
$ cargo build   #build minidecaf
$ cd ..
```


## download/autotest testsuits for minidecaf
```
## let minidecaf-test and minidecaf in the same DIR.
 
$ git clone https://github.com/decaf-lang/minidecaf-tests.git
$ cd minidecaf-tests

## EDIT check.sh, uncomment&change  LINE 15 to  " ../minidecaf/target/debug/minidecaf $cfile > $asmfile "

$ ./check.sh  #test minidecaf
```
