/*file created 10-06-2025
--- TEST ---
this file is an addon to ConsoleApplication2.cpp ; it cannot be used without it
the goal is to add this addon and test it after I refresh the ConsoleApplication2.cpp on github
ConsoleApplication2.cpp is missing (on github) :
- """"optimized"""" parsing, dividing time taken by about 9 iirc
- optimized school multiplication which divided time taken for school multiplication by ~10-20
(adding to final number only once per j instead of once per k)
(adding in a for loop instead of in a for in a for)
- a way to limit operations done by specifically not doing conversion back to string, which was taking the most time
(to test w/ bigger numbers)
[this section is for things I should add, which is not on my main PC yet]
- a new way to stretch my number :
instead of doing a very long char* that i then parse, just copy over and over again a parsed number until it reaches 1GB
(eco of many minutes* ; cons : i can't check if the number is right, because i can't know what both numbers will look like)
(*than if I were to parse an actual GB long input)
- revision of the minimum size to use karatsuba algorithm (because naive became so fast -> worth using for bigger numbers)


This code section is desinged to quicken the parsing by dividing to conquer
Now, I'll just have to test it after updating the base programm on github

Goal :
- if length of input > some_constant : divide the input in 2 (a1 and a2 for exemple) and recall same function to treat these numbers
(a1 = a (input) ; a2 = a+split (split = length_of_input/2) | which means a1 is length split and a2 is length (length_of_input+1)/2
- now we know that a = a1 * 10^split + a2, so we 'just' need to multiply a1 by 10^split
- we also know that both should be same size (for 'Pros and cons considering everything') -> multiplication of 2 numbers size 0.5n
- to calculate 10^split :
{ create 2 numbers, one final and the other intermidiate
  final will start at 1 and intermidiate at 10
  intermidiate will always be 10^(2^something)) -> just multiply final by intermidiate if needed
  how to know if needed ? -> we have split, so just do if (split%2 == 1) and after that, split = split >> 1
}
- return number
- if length_of_input <= some_constant : use already here parsing function (that's why ConsoleApplication2.cpp is needed) 

Theory : 
- dividing the input by 2 each times means I divide into 2 char* of size 0.5n -> (0.5n)² = n²/4 (and because there's 2, that means 2 times less time)

Practice :
- if I split the input in 2, (let's say a1 and a2), to get my number back, i need to multiply a1 by 10^split
- the split can be a huge number, which means I'll be doing multiplications with 2 numbers of size 0.5n,
  which means 'only' 3 times faster than multiplication of a*b

Pros and cons considering everything :
- the multiplication of a*b will be sooooo much faster at higher sizes than the multiplications might just become irrelevant
- although making the 10^split is a repetitive multiplication, which might become very long at last 
(-> constant c in O() very big, which means usable only after too much)
- very memory efficient, as there's no copy, and 10^length only calculated at the end
*/


//starting code
uint64_t some_constant = 50000 //sets a minimum (might be reworked after tests)

Number test_opti_parsing(char* a, uint64_t length) {
   //length is length of a
   //doesn't reset the input
   if (length < some_constant) {
	return parse_base10_str(a); //[Something to rework : a will probably be considered as full char* (-> use of strlen) which might create errors]**
	//** to rework this : might do a new type of a char* with a length
   }
   uint64_t split = length/2;
   char* a1 = a, a2 = a+split;
   uint64_t a1_length = split, a2_length = (length+1)/2;
   Number first_half = test_opti_parsing(a1,a1_length), second_half = test_opti_parsing(a2,a2_length);
   
   //do the 10^split
   Number final = Create_Number(1,1), intermidiate = Create_Number(1,10);
   while (split > 0) {
	if (split%2 == 1) {
		final = Multiplication_Numbers(final,intermidiate); 
	}
	split = split >> 1; //split /= 2
   	intermidiate = Multiplication_Numbers(intermidiate,intermidiate); //intermidiate = intermidiate²
   }
   Number r = Addition_Numbers(Multiplication_Numbers(first_half,final),second_half);
   Free_Number(first_half);
   Free_Number(second_half);
   Free_Number(final);
   Free_Number(intermidiate);
   return r;
}
   