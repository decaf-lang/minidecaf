Unfortunately, 64 bit RISC-V is not directly compatible with 32 bit RISC-V. The semantics of `add`, and other arithmetic instructions change to work on the whole 64 bit registers rather than just the 32 bits that the 32 bit version would operate on. The "w" suffix instructions, mimic the behavior of the 32 bit versions of instructions and sign extend the top 32 bits.

Shifts are also a little different. Shifts consider 1 more bit to determine how far to shift. This allows them to shift an extra 32 bits. Immediate shifts allow encoding an additional bit as well. 

### 64 bit only instructions

| Example Usage | Description |
|---------------|-------------|
|addiw t1,t2,-100|Addition immediate: set t1 to (t2 plus signed 12-bit immediate) using only the lower 32 bits|
|addw t1,t2,t3|Addition: set t1 to (t2 plus t3) using only the lower 32 bits|
|divuw t1,t2,t3|Division: set t1 to the result of t2/t3 using unsigned division limited to 32 bits|
|divw t1,t2,t3|Division: set t1 to the result of t2/t3 using only the lower 32 bits|
|fcvt.d.l f1, t1, dyn|Convert double from long: Assigns the value of t1 to f1|
|fcvt.d.lu f1, t1, dyn|Convert double from unsigned long: Assigns the value of t1 to f1|
|fcvt.l.d t1, f1, dyn|Convert 64 bit integer from double: Assigns the value of f1 (rounded) to t1|
|fcvt.l.s t1, f1, dyn|Convert 64 bit integer from float: Assigns the value of f1 (rounded) to t1|
|fcvt.lu.d t1, f1, dyn|Convert unsigned 64 bit integer from double: Assigns the value of f1 (rounded) to t1|
|fcvt.lu.s t1, f1, dyn|Convert unsigned 64 bit integer from float: Assigns the value of f1 (rounded) to t1|
|fcvt.s.l f1, t1, dyn|Convert float from long: Assigns the value of t1 to f1|
|fcvt.s.lu f1, t1, dyn|Convert float from unsigned long: Assigns the value of t1 to f1|
|fmv.d.x f1, t1|Move float: move bits representing a double from an 64 bit integer register|
|fmv.x.d t1, f1|Move double: move bits representing a double to an 64 bit integer register|
|ld t1, -100(t2)|Set t1 to contents of effective memory double word address|
|lwu t1, -100(t2)|Set t1 to contents of effective memory word address without sign-extension|
|mulw t1,t2,t3|Multiplication: set t1 to the lower 32 bits of t2*t3 using only the lower 32 bits of the input|
|remuw t1,t2,t3|Remainder: set t1 to the remainder of t2/t3 using unsigned division limited to 32 bits|
|remw t1,t2,t3|Remainder: set t1 to the remainder of t2/t3 using only the lower 32 bits|
|sd t1, -100(t2)|Store double word : Store contents of t1 into effective memory double word address|
|slli t1,t2,33|Shift left logical : Set t1 to result of shifting t2 left by number of bits specified by immediate|
|slliw t1,t2,10|Shift left logical (32 bit): Set t1 to result of shifting t2 left by number of bits specified by immediate|
|sllw t1,t2,t3|Shift left logical (32 bit): Set t1 to result of shifting t2 left by number of bits specified by value in low-order 5 bits of t3|
|srai t1,t2,33|Shift right arithmetic : Set t1 to result of sign-extended shifting t2 right by number of bits specified by immediate|
|sraiw t1,t2,10|Shift right arithmetic (32 bit): Set t1 to result of sign-extended shifting t2 right by number of bits specified by immediate|
|sraw t1,t2,t3|Shift left logical (32 bit): Set t1 to result of shifting t2 left by number of bits specified by value in low-order 5 bits of t3|
|srli t1,t2,33|Shift right logical : Set t1 to result of shifting t2 right by number of bits specified by immediate|
|srliw t1,t2,10|Shift right logical (32 bit): Set t1 to result of shifting t2 right by number of bits specified by immediate|
|srlw t1,t2,t3|Shift left logical (32 bit): Set t1 to result of shifting t2 left by number of bits specified by value in low-order 5 bits of t3|
|subw t1,t2,t3|Subtraction: set t1 to (t2 minus t3) using only the lower 32 bits|

### 64 bit only psuedo-instructions:

| Example Usage | Description |
|---------------|-------------|
|fcvt.d.l  f1, t1         |Convert double from signed 64 bit integer: Assigns the value of t1 to f1|
|fcvt.d.lu f1, t1         |Convert double from unsigned 64 bit integer: Assigns the value of t1 to f1|
|fcvt.l.d  t1, f1         |Convert signed 64 bit integer from double: Assigns the value of f1 (rounded) to t1|
|fcvt.l.s  t1, f1         |Convert signed 64 bit integer from float: Assigns the value of f1 (rounded) to t1|
|fcvt.lu.d t1, f1         |Convert unsigned 64 bit integer from double: Assigns the value of f1 (rounded) to t1|
|fcvt.lu.s t1, f1         |Convert unsigned 64 bit integer from float: Assigns the value of f1 (rounded) to t1|
|fcvt.s.l  f1, t1         |Convert float from signed 64 bit integer: Assigns the value of t1 to f1|
|fcvt.s.lu f1, t1         |Convert float from unsigned 64 bit integer: Assigns the value of t1 to f1|
|li t1,1000000000000000 |Load Immediate : Set t1 to 64-bit immediate|