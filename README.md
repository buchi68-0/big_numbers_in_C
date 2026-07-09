# big_numbers_in_C

have ints that can fill up to the full RAM

## Features

Can do basic operation, such as addition, subtraction and multiplication ; division insn't here yet, and might never be
Can convert a big number into a string and the other way around

## Feel free to help me by doing a pull request

this is some issues I know there is (mostly things that are taking too much time) :
parsing base10 string to Number (trying to optimize right now)
converting Number to base10 string

## News

### New algorithm just dropped!

Instead of the old karatsuba, I now implemented the toom-cook algorithm, achieving WAY faster times.  
The algorithm is O(n^(log(5)/log(3))) and even though it's way more complex, it crushes karatsuba but there's some best case scenario that causes it to sometime be way faster and other be mediumly faster
you can see the performance improvements under the Efficiency tab

### Project remake (again) (July 8)

The project has been remade, again, in July of 2026, but this time without any coding style whatsoever, even though I'm trying to make clean code. Most of the project is still coding style, but there's some comments describing how things work, variables being declared not in the beginning of the function for clarity (the variable is created just before being used, so you know when and why it got created)

### Project is now a library instead of just files (July 8)

The project has changed shapes: now instead of compiling a program that used the bigint library, you now can compile a static library (.a file) that contains all the functions. You may however still compile a test to benchmark things.

### Project has gone faster (July 8)

The project has gone very fast, using some pretty hard optimizations: memory optimizations. Basically, before, I was simply doing my recursive function, which spitted out a new number, which I then used and free. But the recursion got so far, that it was doing Millions of allocs and frees for a simple multiplication. Now, there's barely any allocation: the recursive function doesn't allocate anything, it is a wrapper that allocates the result, and a "workspace" of size O(n). The size is 8 * MAX(a_len, b_len). Anyway for the multiplication of 2 numbers, there will be: 4 allocs for the number (1 for the structure, one for the number's limbs, repeat twice), 1 alloc for the result, 1 for the workspace, 3 for the conversion into a string (returned string, and a copy of the original number, so 2) which means 9 allocation, no matter the size, while before it was about 1 Million, pretty neat.

> [!NOTE]
> "pretty hard optimizations" is only because you have to trust the memory optimizations and hope it doesn't take more and more memory with recursion

## Efficiency

### No optimizations

- Multiplication alone of 2 Numbers from raw 200kB values takes about 0.15 (best case) to 0.3 seconds crushing Karatsuba's time of 0.5s (that would be the Number from the parsing of a 481 thousands digits string)
- Multiplication alone of 2 Numbers from raw 4MB values takes 8 (best case) to 25 seconds, crushing Karatsuba's time of 61 seconds (that would be the Number from the parsing of a 9.63MB string)
- converting a 200kB number into string takes 25 seconds, and you can expect 40 minutes for a 2MB number
- parsing a 200kB string takes about 1 second (0.95 in my testing), and parsing a 2MB string takes 95 seconds

> [!NOTE]
> This test has been made on my charging, default settings laptop, with an Intel Core (ultra) 5 120U, performances may change according to your config

### with optimization

Here is a list of the multiplication performances, with optimizations

- 200kB of Raw data (50k limbs): Toom Cook 0.045s (best case) 0.1s (worst case); karatsuba 0.13s
- 4MB of Raw data (1M limbs): Toom Cook 2.00s (best case) 7.5s (worst case); karatsuba 15.05s

## Algorithms

parsing and conversions uses simple O(n²) algorithms which are just dividing either the string or the number by a constant (2^32 for the string, 10^9 for the number) and filling a string, or a bigint's limbs accordingly  

The multiplication uses multiple algorithm depending on the input size, with bigger sizes allowing for more complex but faster algorithms  
Current algorithms implemented

- Schoolbook multiplication. Classical multiplication where you multiply each digit by one another (e.g. 12\*24 = 2\*4 + 2\*20 + 10\*4 + 10\*20). O(n²); where number of limbs <= 30
- Karatsuba algorithm, cutting numbers in half, and making 3 sub-multiplication taking 1/4th of the time to do the full operation. This process can be repeated using recursion and is O(n^(log(3)/log(2))) or about O(n^1.585) ; where number of limbs <= 200
- Toom-Cook algorithm, cutting number in thirds, and making 5 sub multiplication taking 1/9th of the time to do the full operation. This process can be repeated using recursion and achieves O(n^(log(5)/log(3))) or about O(n^1.464) ; where number of limbs > 200

## Quick start

The project is given with a standard Makefile. You may use it to compile

### Linux and Mac users

Because it's a totally normal GNU makefile, you may compile the static library using

```bash
make
```

#### Makefile rules

the rule "fclean" cleans the project with the library's object (.o) files and the library file (.a)  
the rule "clean" cleans the project with the library's object (.o) files but not the static library (.a) file  
the rule "re" recompiles the library, cleaning the project first  
the rule opti recompiles the library with -O3 optimizations  

Reminder: you can use these rules simply by executing

```bash
make [rule]
```

#### Compiling the test

A test is available in main.c, does simple operations and then computes a huge test. Right now the test is just a big number filled to the brim with 0xFFFFFFFF  
to compile this test, you may run  

```bash
make test
```

and to compile the test with optimizations, you may run

```bash
make optest
```

### Windows users

Windows is not really made for this kind of things, but I still shall give you solutions to compile your project.

#### Method 1: make a CMake project

In Visual Studio 2022, you can create a new project (or use an existing one if you have one)  
then, you may copy the files in here into a directory in your project  
And finally, when doing the CMakeLists.txt, just compile the files into a static library  

```cmake
ADD_LIBRARY( BigIntLib STATIC
    [...]/bigint_conv.c
    [...]/bigint_utils.c [...])

ADD_EXECUTABLE( [NAME], [your executable])

TARGET_LINK_LIBRARIES( [NAME]
    BigIntLib)
```

#### Method 2: continue with a C++ project

In Visual Studio 2022, the CMake is being made by itself and is hard to really get into, but i'll still try my best  
The method is quite simple: just add the projects file into your project  
To do that, you can't just copy and paste the files, you also need to add these files in the compilation process. So in the file, you can click _add existing item_ and then add the projects files you previously copied  
