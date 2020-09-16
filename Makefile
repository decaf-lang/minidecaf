ANTLR_JAR ?= /usr/local/lib/antlr-4.8-complete.jar
i ?= i.c
o ?= o.s
CC = riscv64-unknown-elf-gcc  -march=rv32im -mabi=ilp32
QEMU = qemu-riscv32

ifdef DEBUG
	EXTRA_ARGS = -backtrace
else
	EXTRA_ARGS =
endif

RUNMD = python3 -m minidecaf $(EXTRA_ARGS)

CLASSPATH = $(ANTLR_JAR):generated

all: run

run: asm
	$(CC) $(o)
	$(QEMU) a.out ; echo $$?

cst: grammar-java
	java -cp $(CLASSPATH) org.antlr.v4.gui.TestRig MiniDecaf prog -gui $(i)

justrun:
	$(CC) $(o)
	$(QEMU) a.out ; echo $$?

asm: grammar-py
	$(RUNMD) $(i) $(o)

ir: grammar-py
	$(RUNMD) -ir $(i)

ni: grammar-py
	$(RUNMD) -ni $(i)

ty: grammar-py
	$(RUNMD) -ty $(i)

lex: grammar-py
	$(RUNMD) -lex $(i)

parse: grammar-py
	$(RUNMD) -parse $(i)

usage: grammar-py
	$(RUNMD) -h

grammar-py:
	cd minidecaf && java -jar $(ANTLR_JAR) -Dlanguage=Python3 -visitor -o generated MiniDecaf.g4

grammar-java:
	cd minidecaf && java -jar $(ANTLR_JAR) -o ../generated MiniDecaf.g4
	javac -cp $(CLASSPATH) generated/*.java

clean:
	rm -rf generated minidecaf/generated
	rm -rf minidecaf/**__pycache__

.PHONY: all clean run justrun cst ir ni lex parse usage  grammar-py grammar-java
