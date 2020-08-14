ANTLR_JAR ?= /usr/local/lib/antlr-4.8-complete.jar
i ?= i.c
o ?= o.s

CLASSPATH = $(ANTLR_JAR):generated

all: run

run: asm
	riscv64-unknown-elf-gcc $(o)
	qemu-riscv64 a.out ; echo $$?

cst: grammar-java
	java -cp $(CLASSPATH) org.antlr.v4.gui.TestRig MiniDecaf prog -gui $(i)

asm: grammar-py
	python3 -m minidecaf $(i) $(o)

ir: grammar-py
	python3 -m minidecaf -ir $(i)

ni: grammar-py
	python3 -m minidecaf -ni $(i)

usage: grammar-py
	python3 -m minidecaf -h

grammar-py:
	cd minidecaf && java -jar $(ANTLR_JAR) -Dlanguage=Python3 -visitor -o generated MiniDecaf.g4

grammar-java:
	cd minidecaf && java -jar $(ANTLR_JAR) -o ../generated MiniDecaf.g4
	javac -cp $(CLASSPATH) generated/*.java

clean:
	rm -rf generated minidecaf/generated
	rm -rf minidecaf/**__pycache__

.PHONY: all clean run cst ir ni usage  grammar-py grammar-java
