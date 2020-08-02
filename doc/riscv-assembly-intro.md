The [first tutorial](Creating-Hello-World) doesn't explain enough of the core concepts needed to move on to more complicated programs. This page will dig deeper into RISC-V, and explain the fundamentals required for you to write your own "hello world" program.

## Registers

Registers are the variables of assembly. However, unlike in high level languages, we have a limited number (32) of registers. Registers are used to store information that will be needed quickly; long-lasting values generally are stored in RAM. 

In RISC-V, we can only directly manipulate registers. If you want to modify something in RAM you must load it into a register, modify it, and then write the value back to RAM.

There are two ways to refer to each register: its proper name `x0`, `x1`, ..., `x31` and its [ABI](https://en.wikipedia.org/wiki/Application_binary_interface) name `zero`, `ra`, ..., `t6`. The ABI names refer to things like "return address" and "temporary 6". For more information about that, check out the [calling convention page](Calling-Convention). The only special register is `x0`/`zero`, which always has the value 0 and ignores being written to.

## Instructions

All code that is executed in a RISC-V processor comes in the form of instructions. Things like adding/multiplying two numbers, loading a value from RAM into a register, and issuing a system call are instructions. There is no fundamental concept of a function, loop, or if statement; all of those concepts are made using instructions. 

For example, the following executes the `addi` instruction 10 times.

```
  li t0, 10         # t0 = 10
  li t1, 0          # t1 = 0
loop:
  addi t1, t1, 1    # add t1 += 1
  bne t0, t1, loop  # Branch to the loop label if t0 == t1 
```

A full list of instructions supported by RARS is available [here](Supported-Instructions).

### Immediates and Labels

Nearly every instruction involves a register, but some also use a number or label. This is often referred to as an immediate. Immediates are stored as part of the instruction in the form of a number. When assembling the program, the label's address - the instruction's address is used as the immediate.  

The use of labels like `loop` in the small program above is not strictly necessary. You could write the instructions directly doing that, but it would be very time consuming to write. 

_Note: RARS doesn't actually support hardcoding offset immediates, but it would work in GCC._

### Psuedo-Instructions

RISC-V is a [RISC](https://en.wikipedia.org/wiki/Reduced_instruction_set_computer) instruction set, and therefore tries to minimize instructions. So there isn't an instruction dedicated to loading a value into a register, or to move a value from one register to another. However, many instructions that might be nice to have can be made using other instructions. We call these instructions (that aren't in the instruction set, but can be directly replace with real instructions) psuedo-instructions.

For example, `li t0, 10` can be translated into `addi t0, zero, 10`.

## Assembler Directives 

Assembler directives are a way of telling the assembler what to put in the executable besides instructions. It allows you to set up memory for when your program starts running. All directives start with a `.` like `.data` or `.macro`.   

I'll explain the most common ones, but a full list of the directives is available [here](Assembler-Directives).

### Text vs Data

Easily the most important directives are `.text` and `.data`; these tell the assembler where to store the instructions or data that follow. The `.text` section stores instructions and the `.data` section stores (global) variables we will be using and starts with them initialized.

Because code and data become bytes before a program starts, we need to tell the assembler in what section to put code. Both directives can also be used to tell the assembler exactly where to put data/code in the section. This can be done by appending an address after the directive like `.text 0x400010`, but this is generally not necessary.  

There are also subtle differences between the `.data` and `.text` sections that can be disabled in RARS by enabling self-modifying code. More about that can be found in [a later explanation](Sections-and-Permissions).

### Initializing data

Generally, there are four types of data that can be set up after a `.data` directive: integers, floating point numbers, strings, and allocated space for the program to fill later.

Integers can be initialized with `.byte`, `.half`, and `.word` for 1 byte, 2 byte, and 4 byte integers, respectively. They can also store multiple comma-separated values.

For example:
```
.data
big:
  .word 0x1000000
medium:
  .half 0x1000
smalls:
  .byte 0x10, 90, -10
```

Floating point numbers can be initialized with `.float`. The number is stored as the closest IEEE 754 floating point number. `NaN` and `Inf` can be used to represent "Not a Number" and Infinity, respectively. Like with integers, it can store multiple values.

For example:
```
.float 90, NaN, -Inf, -1.337, 1e10
```

There are two flavors of strings: those automatically ending with a NULL (0) byte, and those that are not. `.asciz` or `.string` is used for the former, and `.ascii` is used for the latter. Despite what the names would imply, the directives actually store the strings encoded as [UTF-8](https://en.wikipedia.org/wiki/UTF-8) characters. However, because UTF-8 is backward-compatible with [ASCII](https://en.wikipedia.org/wiki/ASCII), the correct characters/strings are stored.

For example, the following all put the same string into memory:
```
ASCIZ:
.asciz "Hello"
ASCII:
.ascii "Hello\0"
BYTES:
.byte 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0
.
```

Finally, the last way to set up memory is to allocate space only filled with zeroes. The main way to do this is using the `.space` directive. `.space 10` will allocate ten bytes of space. The other way is to use `.extern`; it allows multiple files to define the same variable without duplicating it. This would generally be used in a file that is included using `.include` so that multiple files including the same file don't duplicate it in memory.

```
.extern LABEL, 90 # allocates 90 bytes that can be referenced by LABEL
``` 




