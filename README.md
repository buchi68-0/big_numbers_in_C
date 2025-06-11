# big_numbers_in_C
have ints that can fill up to the full RAM

# Features
Can do basic operation, such as addition, subtraction and multiplication ; division insn't here yet, and might never be
Can parse a string into binary, and do the operation the other way around

# Feel free to help me by doing a pull request !
this is some issues I know there is (mostly things that are taking too much time) :
parsing base10 string to Number (trying to optimize right now)
converting Number to base10 string

# the programm might not be refreshed from what I've done on my PC at home
problems in the main branch right now might be already fixed on my PC

# this is a project to make me learn C
It isn't supposed to be fully optimized
It only uses Karatsuba algorithm for multiplication (and not the last-discovered ones)

# Efficiency :
- Multiplication alone of 2 Numbers from 1 MiB char* takes 2.2s
- parsing is taking 15.1s in the same conditions
- convertion back to string should be about 20s for each Numbers and 80 for multiplication (I made an option to skip string conversion because it's the thing taking the most time)
(data could be for 8 MiB strings, because I don't remember exact conditions of the programm)
(my PC CPU is an intel-Core i7-3770 @3.4GHz, which is getting old right now)

# future tests :
- it is planned that I do a 128MiB number multiplication, stretching a binary number instead of stretching a string that i then parse to gain time (parsing is O(n²), so multiplying the input by 128 might multiply time by 15_000+ and I don't have 5 hours for just some parsing) ;  expected time is 2 hours
