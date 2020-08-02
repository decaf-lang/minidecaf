The currently supported directives are:

| Name | Description|
|------|------------|
|.align|Align next data item on specified byte boundary (0=byte, 1=half, 2=word, 3=double)|
|.ascii|Store the string in the Data segment but do not add null terminator|
|.asciz|Store the string in the Data segment and add null terminator|
|.byte|Store the listed value(s) as 8 bit bytes|
|.data|Subsequent items stored in Data segment at next available address|
|.double|Store the listed value(s) as double precision floating point|
|.end_macro|End macro definition.  See .macro|
|.eqv|Substitute second operand for first. First operand is symbol, second operand is expression (like #define)|
|.extern|Declare the listed label and byte length to be a global data field|
|.float|Store the listed value(s) as single precision floating point|
|.globl|Declare the listed label(s) as global to enable referencing from other files|
|.half|Store the listed value(s) as 16 bit halfwords on halfword boundary|
|.include|Insert the contents of the specified file.  Put filename in quotes.|
|.macro|Begin macro definition.  See .end_macro|
|.section|Allows specifying sections without .text or .data directives. Included for gcc comparability|
|.space|Reserve the next specified number of bytes in Data segment|
|.string|Alias for .asciz|
|.text|Subsequent items (instructions) stored in Text segment at next available address|
|.word|Store the listed value(s) as 32 bit words on word boundary|
