# minidecaf

## setup environment
1. install rustc tools
```
curl https://sh.rustup.rs -sSf | sh
rustup target add riscv64imac-unknown-none-elf
```
2. install riscv64-gcc tools
https://github.com/xpack-dev-tools/riscv-none-embed-gcc-xpack/releases/download/v8.3.0-1.2/xpack-riscv-none-embed-gcc-8.3.0-1.2-linux-x64.tar.gz

3. install qemu
```
sudo apt-get install qemu
```

## build/run/test
### build
cargo build
### run
cargo run -- "0;"
### test
. ./test/test.sh 

