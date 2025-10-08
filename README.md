# big_numbers_in_C
have ints that can fill up to the full RAM

## Features
Can do basic operation, such as addition, subtraction and multiplication ; division insn't here yet, and might never be
Can parse a string into binary, and do the operation the other way around

## Feel free to help me by doing a pull request !
this is some issues I know there is (mostly things that are taking too much time) :
parsing base10 string to Number (trying to optimize right now)
converting Number to base10 string

## Efficiency :
- Multiplication alone of 2 Numbers from 1 MiB char* takes 2.2s
- parsing is taking 15.1s in the same conditions
- convertion back to string should be about 20s for each Numbers and 80 for multiplication (I made an option to skip string conversion because it's the thing taking the most time)

# News :
- recoded the whole thing in C, according to Epitech's coding style (20 lines per function, max 10 per file)
- features are now separated in files depending on what they do :
    - Number_creation_procedures allows for basic creation and allocation of numbers
    - Number_conversion_procedures allows for parsing base 10 string into binary number
    - Number_printable_procedures allows for converting binary number into base 10 string
    - Number_basic_operation_procedures allows for basic opereration, such as addition and subtraction
    - Number_mul_procedures allow for multiplication with 2 algorithm, one O(n²) and one O(n^(log(3)/log(2)))
- files depends on others to work. Here's the list :
    - everyone depends on the Number_type.h (of course)
    - all except for the one itself requires Number_creation_procedures
    - Number_mul_procedures depends on Number_basic_operation_procedures also.
- there's a makefile to compile. In : test_C/Makefile (everything will be putted back in root when finished)

# Use the makefile :
- for Linux (and maybe Mac) users :
    - make alone compiles with clang
    - for gcc users, there's a gccver rule
    - make debug launches debug for myself, and as such uses epiclang which i'm pretty sure you don't have
    - make fsanit launches with fsanitize=address; no leaks inside the whole programm (I'm a good programmer, i know) (fsanit is compiled with clang)
- for Microsoft Visual Studio 2022 users :
    - create a new project (no matter the name)
    - add all files manually (file, add file, .c(pp)) with name of files in this project
    - then you have two choices :
        - add your own main inside $(PROJECT_NAME).c(pp) and so remove test01.C
        - copy test01.c into $(PROJECT_NAME).c(pp)
    - If you know Visual Studio more than that, you can instead :
        - get into compiler config
        - add manually all source (.c) files into the compiler's paths
        - don't forget to do that for both release and debug
    - And then, you can click the compile and run button, and it should work


    

