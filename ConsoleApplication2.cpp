//projet calculatrice qui prend toute la RAM

#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#pragma warning(disable : 4996)
#include <Windows.h>
#define CConsole true

void CreateConsoleWindow() {
    AllocConsole();  // Create a console window
    FILE* fileStream;
    freopen_s(&fileStream, "CONOUT$", "w", stdout);  // Redirect stdout to the console
    freopen_s(&fileStream, "CONOUT$", "w", stderr);  // Redirect stderr to the console
}
#endif
#ifndef CConsole
#define CConsole false
void CreateConsoleWindow() return;
#endif

typedef struct Number {
    uint32_t* Value;
    uint64_t length;
    bool sign;
};


void FreeNumber(Number _Tofree) {
    if (_Tofree.Value == NULL) return;
    if (_CrtIsValidHeapPointer(_Tofree.Value)) {
        free(_Tofree.Value);
        _Tofree.Value = NULL;
        _Tofree.length = 0;
    }
}
//faire un signed number : il faut juste ajouter un sign qui vaut 1 ou 0 suivant s'il est négatif ou pas
/*l'addition/soustraction reste simple :
positif + positif : rien ne change
positif - positif : rien ne change (si le nombre 1 est supérieur au nombre 2 sinon on mets le sign puis on ajoute au lieu de reduire)
positif + négatif = positif - positif
negatif + positif = -(positif - positif)
positif - négatif = positif + positif
negatif - negatif = -(positif - positif)
negatif + negatif = -(positif + positif)*/
//Idée : faire des fonctions cachées pour chaque cas (puisque le négatif peuvent toujour se changer en positif pour faire l'opération)
//bref

void PanickError(const char* msg) {
    printf("\nError : %s\n", msg);
}
void PanickError(char* msg) {
    printf("\nError : %s\n", msg);
}

uint64_t minimum_for_karatsuba = 30;

Number Create_Number(const uint64_t base_length) {
	//creates a Number of base_length
    Number r = { (uint32_t*)calloc(base_length,4), base_length, false };
    if (r.Value == NULL) {
        PanickError("allocation of r in function Create_Number failed");
        printf("Ending programm...\n");
        exit(0x10001);
    }
    return r;
}
Number Create_Number(const uint64_t base_length, const uint32_t first_value) {
    //sets the first value
    Number r = { (uint32_t*)calloc(base_length,4), base_length, false };
    if (r.Value == NULL) {
        PanickError("allocation of r in function Create_Number failed");
        printf("Ending programm...\n");
        exit(0x10001);
    }
    if (base_length > 0) r.Value[0] = first_value;
    return r;
}

void Check_for_useless_zero(Number * _Arg) {
    uint64_t retest = 0;
    for (uint64_t i = 0; (*_Arg).Value[i] == 0 && i < (*_Arg).length; i++) {
        retest++;
    }
    if (retest == (*_Arg).length) {
        FreeNumber((*_Arg));
        (*_Arg) = Create_Number(0);
        return;
    }
    else if (retest != 0) {
        (*_Arg).length -= retest;
        for (uint64_t i = 0; i < (*_Arg).length; i++) {
            (*_Arg).Value[i] = (*_Arg).Value[i + retest];
        }
        (*_Arg).Value = (uint32_t*)realloc((*_Arg).Value, (*_Arg).length * sizeof(uint32_t));
        if ((*_Arg).Value == NULL) { //checking if reallocation worked
            PanickError("Reallocation of result in function _Addition_Unsigned failed");
            printf("Ending programm...\n");
            exit(0x10001);
        }
        return;
    }
    return;
}


// ----------- UNUSED -----------------

//Number constant_10power9 = Create_Number(1, 1000000000);
//Number constant_10 = Create_Number(1, 10);

bool Equal(const Number Var1, const Number Var2) {
    //
    int64_t difference = Var1.length - Var2.length;
    for (int64_t i = Var2.length; i > 0; i--) {
        if (Var1.Value[i + difference] != Var2.Value[i]) return false;
    }
    if (difference > 0) {
        for (int64_t i = 0; i < difference; i++) {
            if (Var1.Value[i] != 0) return false;
        }
    }
    else {
        for (int64_t i = 0; i < abs(difference); i++) {
            if (Var2.Value[i] != 0) return false;
        }
    }
    return true;
}

uint64_t uclen(unsigned char* input_) {
    uint64_t returned = 0;
    while (true) {
        if (input_[returned] == '\0') {
            return returned;
        }
        returned += 1;
    }
}
// ----------- END UNUSED -------------

Number Max(const Number Var1, const Number Var2) {
    //Doesn't care about the sign
    //Is actually used now
    if (Var1.length > Var2.length) {
        uint64_t difference = Var1.length - Var2.length;
        for (uint64_t i = 0; i < difference; i++) {
            if (Var1.Value[i] > 0)return Var1;
        }
        for (uint64_t i = 0; i < Var2.length; i++) {
            if (Var1.Value[i + difference] > Var2.Value[i]) return Var1;
            else if (Var1.Value[i + difference] < Var2.Value[i]) return Var2;
        }
    }
    else {
        uint64_t difference = Var2.length - Var1.length;
        for (uint64_t i = 0; i < difference; i++) {
            if (Var2.Value[i] > 0)return Var2;
        }
        for (uint64_t i = 0; i < Var1.length; i++) {
            if (Var2.Value[i + difference] > Var1.Value[i]) return Var1;
            else if (Var2.Value[i + difference] < Var1.Value[i]) return Var2;
        }
    }
    return Var2; //if equal, just return one of them
}


bool format_str_for_parsing(char* _Input, const uint64_t Input_length) {
    //takes an Input (char*) supposed to be a string of digits (i.e : "1234")
    //formats it so it can be used propelly by parse_base10_str (-> {1,2,3,4})
    //returns false if Input not a string of digits ; true else
    for (uint64_t i = 0; i < Input_length; i++) {
        _Input[i] -= '0'; //convert to get {1,2,3,4} for exemple instead of {49,50,51,52}
        if (_Input[i] > '9') return false; //not a digit, return false (hope that if it's going below 0 it comes back to 256)
    }
    return true;
}

void divide_char_by2(char* _Input, const uint64_t Input_length, uint64_t * Number_of_zero) {
    //"divide" _Input by 2 ; used for parsing
    //won't create errors if _Input hasn't 'format_str_for_parsing' format
    //WILL create errors if _Input[Input_length-1] is even (exception : Input_length != strlen(_Input))
    //add to Number_of_zero if new useless '0' detected
    for (uint64_t i = Input_length - 1; i >= (*Number_of_zero) && i < Input_length; i--) {
        // i >= (*Number_of_zero) && i < Input_length :
        //is a security if (*Number_of_zero) is 0 (then; it will always be >= 0 because it's unsigned)
        if (_Input[i] % 2 == 1) { //if even
            _Input[i + 1] += 5; //add 5 to next digit (creates error if i is strlen(_Input-1)
            _Input[i] -= 1;
        }
        _Input[i] = _Input[i] / 2; //dividing by 2
        if (i == (*Number_of_zero) && _Input[i] == 0) { //new zero detected (at the left)
            (*Number_of_zero)++;
        }
    }
}



/* ----------------------------------------------- */

//BASIC OPERATIONS
/*
* The only callable functions are subtraction_Numbers and addiction_Numbers
* Others could make false results or even maybe errors
*/
Number _Subtraction_Unsigned(const Number _A1,const Number _A2) {
    //Child function, DO NOT CALL

    Number Result = Create_Number(_A1.length); //max size : _A1's size
    uint32_t retenue = 0;
    int16_t intermidiate = 0;
    for (int i = 0; i < _A2.length; i++) {
        intermidiate = _A1.Value[_A1.length - i - 1] - _A2.Value[_A2.length - i - 1] - retenue;
        if (intermidiate < 0) {
            retenue = 1;
            intermidiate += 4294967296; //is 2 ^ 32 ; didn't want to do (1 << 32) because I got told it's just 32 times << 1
        }
        Result.Value[Result.length - i - 1] = intermidiate;
    }
    for (int i = _A2.length; i < _A1.length; i++) {
        intermidiate = _A1.Value[_A1.length - i - 1] - retenue;
        if (intermidiate < 0) {
            retenue = 1;
            intermidiate += 4294967296; //is 2 ^ 32 ; didn't want to do (1 << 32) because I got told it's just 32 times << 1
        }
        Result.Value[Result.length - i - 1] = intermidiate;
    }
    return Result;
}
Number _Addition_Unsigned(const Number _A1, const Number _A2) {
    //_A1.length HAS to be SUPERIOR or equal to _A2.length
    //Child function DO NOT CALL

    uint64_t length = _A1.length + 1; //length of result
    Number result = Create_Number(length);
    uint64_t _number = 0; //it's the intermediate for the addition
    for (uint64_t i = 0; i < _A2.length; i++) { //we add normally until we reach the end of one Number
        _number += (uint64_t)_A1.Value[_A1.length - i - 1];
        _number += (uint64_t)_A2.Value[_A2.length - i - 1];
        //we add because of the potential retenue
        result.Value[result.length - i - 1] = _number & 4294967295;  //takes first 32 bits
        _number = _number >> 32; //shifting to keep the retenue
        //(even though yeah the retenue can only be 1 or 0)
    }
    for (uint64_t i = _A2.length; i < _A1.length; i++) {
        _number += (uint64_t)_A1.Value[_A1.length - i - 1];
        result.Value[result.length - i - 1] = _number & 4294967295;
        _number = _number >> 32;
    }
    result.Value[0] = _number;


    //check for useless 0s
    Check_for_useless_zero(&result);
    return result;
}

Number subtraction_Numbers(const Number _Base,const Number _Subtraction) {
    //Does _Base - _Subtraction
    //This is the called function
    if (!(_Base.sign || _Subtraction.sign)) { //positive - positive
        Number max__ = Max(_Base, _Subtraction);
        if (max__.Value == _Base.Value) return _Subtraction_Unsigned(_Base, _Subtraction);
        else {
            Number r = _Subtraction_Unsigned(_Subtraction, _Base);
            r.sign = true; // a-b = -(b-a)
            return r;
        }
    }
    else if (_Base.sign && _Subtraction.sign) {//negative - negative
        //negative - negative = negative + positive = -(positive - positive);
        Number max__ = Max(_Base, _Subtraction);
        if (max__.Value == _Base.Value) {
            Number r = _Subtraction_Unsigned(_Base, _Subtraction);
            r.sign = true;
            return r;
        }
        else {
            return _Subtraction_Unsigned(_Subtraction, _Base);
            //if _Subtraction > ; number will be positive anyway
        }
    }
    else if (_Base.sign) { //negative - positive
        // <==> -(positive + positve)
        //that one's EZ
        if (_Base.length > _Subtraction.length) {
            Number r = _Addition_Unsigned(_Base, _Subtraction);
            r.sign = true;
            return r;
        }
        else {
            Number r = _Addition_Unsigned(_Base, _Subtraction);
            r.sign = true;
            return r;
        }
    }
    else { //positive - negative
        // <==> positive + positive
        //EZ as well
        if (_Base.length > _Subtraction.length) {
            return _Addition_Unsigned(_Base, _Subtraction);
        }
        else {
            return _Addition_Unsigned(_Subtraction, _Base);
        }

    }

}

Number addition_Numbers(Number const _Arg1, Number const _Arg2) {
    //This is the called function.
    //It will then handle signs and sizes with other child functions
    //not const, because the sign of _Arg1 and _Arg2 can change (is reverted after)
    if (_Arg1.sign && _Arg2.sign) { //both negative
        if (_Arg1.length > _Arg2.length) {
            Number r = _Addition_Unsigned(_Arg1, _Arg2);
            r.sign = true; //set to negative (negative + negative = -(positive + positive))
            return r;
        }
        else {
            Number r = _Addition_Unsigned(_Arg2, _Arg1);
            r.sign = true; //set to negative (negative + negative = -(positive + positive))
            return r;
        }
    }
    else if (!(_Arg1.sign || _Arg2.sign)) { //both positive (basic addition)
        if (_Arg1.length > _Arg2.length) {
            return _Addition_Unsigned(_Arg1, _Arg2);
        }
        else {
            return _Addition_Unsigned(_Arg2, _Arg1);
        }
    }
    else if (_Arg1.sign) { //only cases left : only _Arg1 is negative and only _Arg2 is negative
        //negative + positive = -(positive - positive)
        //CAREFULL : the returned value can be negative, so the sign has to be a xor with current sign
		if (Max(_Arg1, _Arg2).Value == _Arg1.Value) {
			//if _Arg1 is the biggest
			//then we do _Arg1 - _Arg2
			//and we set the sign to negative
			Number r = _Subtraction_Unsigned(_Arg1, _Arg2);
			r.sign = true;
			return r;
		}
		else {
			//if _Arg2 is the biggest
			//then we do _Arg2 - _Arg1
			//and we set the sign to positive
			Number r = subtraction_Numbers(_Arg2, _Arg1);
			r.sign = false; //if _Arg2 is bigger (and also is positive) then _Arg1 + _Arg2 is positive
			return r;
		}
    }
    else {
        //positive + negative = positive - positive
        //though it's litterally just a subtraction
        if (Max(_Arg1, _Arg2).Value == _Arg1.Value) {
            //if _Arg1 is the biggest
            //then we do _Arg1 - _Arg2
            //and we set the sign to negative
            Number r = _Subtraction_Unsigned(_Arg1, _Arg2);
            r.sign = false;
            return r;
        }
        else {
            //if _Arg2 is the biggest
            //then we do _Arg2 - _Arg1
            //and we set the sign to positive
            Number r = subtraction_Numbers(_Arg2, _Arg1);
            r.sign = true;
            return r;
        }
    }
}

/* ----------------------------------------------- */

void private_add_assign(Number const _Arg1, Number const _Arg2) {
    //Doesn't optimize memory ; takes 2 same-length Number so I don't have to check the size difference as well
    //Add _Arg2 into _Arg1
    //Has to be used INTO Multiplication_Numbers because it DOESN'T handle different sizes
    //Takes for size the same as the rest
    //Why ? because _Arg1 and _Arg2 have for length the MAXIMUM of the multiplication of the 2 Numbers (in Multiplication_Numbers)
    uint64_t _number = 0; //intermidiate
    for (uint64_t i = _Arg1.length - 1; i < _Arg1.length; i--) {
        _number = _Arg1.Value[i] + _Arg2.Value[i];
		_Arg1.Value[i] = _number & 4294967295; //takes first 32 bits
		_number = _number >> 32; //shifting to keep the retenue
		//(even though yeah the retenue can only be 1 or 0)
        
    }
    return;
}

Number bit_shift(Number _From, const uint64_t number) {

    uint64_t nb_octet = number / 32;
    uint32_t nb_bit = number % 32;

    uint64_t length_ = number / 32 + _From.length;
    if (nb_bit != 0) length_++;
    Number returned = Create_Number(length_); //handles problems
    uint64_t temp[2] = { 0, 0 };
    if (nb_bit != 0) {
        for (uint64_t i = _From.length; i < _From.length + 1; i--) {
            temp[i % 2] = _From.Value[i - 1] << nb_bit;
            returned.Value[i] = temp[i % 2] & 4294967295;
            returned.Value[i] = returned.Value[i] | temp[(i + 1) % 2];
            temp[i % 2] = temp[i % 2] >> 32;
        }
        returned.Value[0] = temp[1]; //the loop will alwyas end at i = 1 so we just do as if i == 0
    }
    else {
        memcpy(returned.Value, _From.Value, _From.length * 4); //if it's moving just bytes, then it's easy
    }
    return returned;
}

// changing -> will become unused
uint64_t Get_Number_of1s(const Number _Arg) {
    //so it's a basic for loop that check for each byte, each bit
    //complexity : O(32n) with n the length of _Arg
    //there'll be problems if the length is 2^61+ (then 32*length is an overflow)
    uint64_t r = 0;
    uint32_t _temp;
    for (uint64_t i = 0; i < _Arg.length; i++) {
        _temp = _Arg.Value[i];
        for (uint64_t j = 0; j <= 7; j++) {
            if (_temp & 1 == 1) r++; //checks if 1 or 0
            _temp = _temp >> 1;
            //got told >> n is n times >>1 so i guess it's better than doing n(n+1)/2 shifting
        }
    }
    return r;
}

// -- Now, let's do a function so multiplication_Numbers only use Number of ones and this function to get the result
//(it makes it easy to implement karatsuba algorithm because i can then check if the number is long enough to cut it in half))
Number private_naive_muliplication_system(Number const _Multiplied, Number const _Multiplier) {
    //basically, multiplication will do the optimal way (take number_of_one, etc.) while this function just multiplies
    //_Multiplied will be the one to be bit shifted according to _Multiplier's 1s positions
    Number Result = Create_Number(_Multiplied.length + _Multiplier.length);//the Result ; maximum length is the addition of both
    //no need to verify, it already handles that in Create_Number
    Number temporary = Create_Number(Result.length);
    uint64_t temp_ = 0;
    //Basic multiplication algorithm, O(n²), won't calculate ones in each numbers again because it's too long
    for (uint64_t k = 0; k < _Multiplied.length; k++) {
        for (uint64_t j = 0; j < _Multiplier.length; j++) {
            temp_ = _Multiplied.Value[_Multiplied.length - k - 1];
            temp_ *= _Multiplier.Value[_Multiplier.length - j - 1];
            temporary.Value[temporary.length - k - j - 1] += temp_ & 4294967295;
            if (temporary.Value[temporary.length - k - j - 1] < (temp_ & 4294967295)) { //if the addition overflowed
				temporary.Value[temporary.length - k - j - 2] += 1; //add the retenue
            }
            if (temp_ >> 32 != 0) {
                temporary.Value[temporary.length - k - j - 2] += temp_ >> 32; // '-k - j' are doing the bit shifting by themselves
                if (temporary.Value[temporary.length - k - j - 2] < (temp_ >> 32)) { //if the addition overflowed
                    temporary.Value[temporary.length - k - j - 3] += 1; //add the retenue
                }
            }
        }
        private_add_assign(Result, temporary); //add temporary to Result
        memset(temporary.Value, 0, temporary.length * 4); //does a memset now instead of allocating    
    }
    Result.sign = _Multiplied.sign ^ _Multiplier.sign; //does a xor cause - * - = + and + * + = + while if only one - it's -
    Check_for_useless_zero(&Result);
    return Result;
}
 
Number* Split(const Number target, uint64_t at) {
    //splits the Input to 2 numbers, cutted down at 'at'
    //r, the return value, will always have a length of 2. r[0] is the low part of target and r[1] is the high part (target = r[1] << where + r[0])
    if (at > target.length) {
        Number* r = (Number*)calloc(2, sizeof(Number)); //did not go for Number r[2] because it was ALWAYS making split1 & split2 (in karatsuba function) THE EXACT SAME
		if (r == NULL) {
			PanickError("allocation of r in function Split failed");
			printf("Ending programm...\n");
			exit(0x10001);
		}
        r[0] = Create_Number(target.length);
        r[1] = Create_Number(0);
        r[1].length = 0; //check so i'm sure
        memcpy(r[0].Value, target.Value, r[0].length * 4);
        r[0].sign = target.sign; r[1].sign = target.sign;
        return r;
    }
    else {
        Number* r = (Number*)calloc(2, sizeof(Number));
		if (r == NULL) {
			PanickError("allocation of r in function Split failed");
			printf("Ending programm...\n");
			exit(0x10001);
		}
        r[0] = Create_Number(at);
        r[1] = Create_Number(target.length - at);
        memcpy(r[0].Value, target.Value + target.length - at, at * 4); //from target+target.length-where to length (where amout of uint32)
        memcpy(r[1].Value, target.Value, r[1].length * 4); //from target to target+where
        r[0].sign = target.sign; r[1].sign = target.sign;
        if (!(_CrtIsValidHeapPointer(r[0].Value) && _CrtIsValidHeapPointer(r[1].Value))) {
            printf("error !");
        }
        return r;
    }
}

Number private_karatsuba_implementation(Number a1, Number a2) {
    //this version frees the args. DO NOT USE OUTSIDE FUNCTION
    //Child function, DO NOT CALL
    if (a1.length < minimum_for_karatsuba && a2.length < minimum_for_karatsuba) {
        //doesn't care about the number of ones anymore
        Number r = private_naive_muliplication_system(a2, a1);
        return r;
    }
    uint64_t temp__ = max(a1.length, a2.length);
    temp__ = temp__ / 2;
    Number* split1 = Split(a1, temp__);
    Number* split2 = Split(a2, temp__);
    //free the numbers
    FreeNumber(a1); //not used after
    FreeNumber(a2); //not used afters
    Number add1 = addition_Numbers(split1[0], split1[1]);
    Number add2 = addition_Numbers(split2[0], split2[1]);
    Number s1 = private_karatsuba_implementation(split1[0], split2[0]); //after that, split1[0] and split2[0] are freed
    Number s2 = private_karatsuba_implementation(add1, add2);
    Number s3;
    if (split1[1].length == 0 || split2[1].length == 0) {
        free(split1);
        free(split2);
        return addition_Numbers(bit_shift(subtraction_Numbers(s2, s1), temp__ * 32), s1);
        // <==> return (s2 - s1)*2^(32^temp__) + s1
    }
    else {
        s3 = private_karatsuba_implementation(split1[1], split2[1]); //after that, split1[1] and split2[1] are freed -> no usage of freed number
        free(split1);
        free(split2);   
        return addition_Numbers(bit_shift(s3, temp__ * 64), addition_Numbers(bit_shift(subtraction_Numbers(subtraction_Numbers(s2, s1), s3), temp__ * 32), s1));
        // <==> return s3*2^(32^(temp__*2)) + (s2 - s1 - s3)*2^(32^temp__) + s1
    }
}

Number test_Multiplication_Numbers(const Number Arg1, const Number Arg2) {
	//Safe : multiplication_Numbers doesn't free the input
    if (Arg1.length < minimum_for_karatsuba && Arg2.length < minimum_for_karatsuba) {
        //doesn't care about 1s anymore
        return private_naive_muliplication_system(Arg1, Arg2);
		//sign already handled by private_naive_muliplication_system
    }
    uint64_t temp__ = max(Arg1.length, Arg2.length);
    temp__ = temp__ / 2;
	//No split : just take the pointer
    Number split1[2];
    if (temp__ > Arg1.length) {
        split1[0] = { Arg1.Value, Arg1.length, Arg1.sign };
        split1[1] = { NULL, 0, Arg1.sign };
	}
	else {
		split1[0] = { Arg1.Value + Arg1.length - temp__, temp__, Arg1.sign };
		split1[1] = { Arg1.Value, Arg1.length - temp__, Arg1.sign };
	}
	Number split2[2];
    if (temp__ > Arg2.length) {
        split2[0] = { Arg2.Value, Arg2.length, Arg2.sign };
        split2[1] = { NULL, 0, Arg2.sign };
    }
    else {
        split2[0] = { Arg2.Value + Arg2.length - temp__, temp__, Arg2.sign };
        split2[1] = { Arg2.Value, Arg2.length - temp__, Arg2.sign };
    }
    //no free : no need to free
    Number add1 = addition_Numbers(split1[0], split1[1]);
    Number add2 = addition_Numbers(split2[0], split2[1]);
    Number s1 = test_Multiplication_Numbers(split1[0], split2[0]); 
    Number s2 = test_Multiplication_Numbers(add1, add2);
    Number s3;
    if (split1[1].length == 0 || split2[1].length == 0) {
        Number r = Create_Number(Arg1.length + Arg2.length);
        memcpy(r.Value + r.length - s1.length, s1.Value, s1.length * 4); //copy s1 to the LO part of r
        Number temp = bit_shift(subtraction_Numbers(s2, s1), temp__ * 32);
        return addition_Numbers(r, temp);
        // <==> return (s2 - s1)*2^(32^temp__) + s1
    }
    else {
        s3 = test_Multiplication_Numbers(split1[1], split2[1]);
        Number r = Create_Number(Arg1.length + Arg2.length);
        memcpy(r.Value + r.length - s1.length, s1.Value, s1.length*4); //copy s1 to the LO part of r
        memcpy(r.Value + r.length - (temp__ * 2) - s3.length, s3.Value, s3.length * 4); //copy s3 to HI part of r
        Number temp = bit_shift(subtraction_Numbers(s2, addition_Numbers(s1,s3)), temp__ * 32);
        return addition_Numbers(r, temp);
        // <==> return s3*2^(32^(temp__*2)) + (s2 - s1 - s3)*2^(32^temp__) + s1
    }
}


Number multiplication_Numbers(const Number Arg1, const Number Arg2) {
    //Called function
    //handles small number with the naive method, else does karatsuba.
    //SAFE : doesn't delete the Input


    //Karatsuba implementation :

    if (Arg1.length < minimum_for_karatsuba && Arg2.length < minimum_for_karatsuba) { //The 100 here is totally arbritrary. It got chosen because it was the fastest between 10, 100 and 1000
		Number r = private_karatsuba_implementation(Arg1, Arg2);
		//sign handled by private_karatsuba_implementation (more precisely, by naive_multiplication_system used in karatsuba)
        return r;
    }

    else {
        uint64_t temp__ = max(Arg1.length, Arg2.length);
        temp__ = temp__ / 2;
        Number* split1 = Split(Arg1, temp__);
        Number* split2 = Split(Arg2, temp__);
        Number add1 = addition_Numbers(split1[0], split1[1]);
        Number add2 = addition_Numbers(split2[0], split2[1]);
        Number s1 = private_karatsuba_implementation(split1[0], split2[0]);
        Number s2 = private_karatsuba_implementation(add1, add2);
        Number s3;
        if (split1[1].length == 0 || split2[1].length == 0) {
            return addition_Numbers(bit_shift(subtraction_Numbers(s2, s1), temp__ * 32), s1);
            // <==> return (s2-s1)*2^(32^temp__) + s1
        }
        else {
            s3 = private_karatsuba_implementation(split1[1], split2[1]);
            return addition_Numbers(bit_shift(s3, temp__ * 64), addition_Numbers(bit_shift(subtraction_Numbers(subtraction_Numbers(s2, s1), s3), temp__ * 32), s1));
            // <==> return s3*2^(32^(temp__*2)) + (s2 - s1 - s3)*2^(32^temp__) + s1
        }
    }
}

/* ----------------------------------------------- */

char* convert_to_str(const Number from_) {
    //BASE 16 REPRESENTATION
    // --- UNUSED ---
    // UNUSABLE !
    char* returned = (char*)(calloc(from_.length * 2 + 2, sizeof(char)));
    if (returned == NULL) {
        exit(47);
    }
    returned[0] = '0'; //le "0x" au début n'est pas obligatoire, mais je l'aime bien là où il est
    returned[1] = 'x';
    int a = from_.length - 1;
    char constants[16] = { '0','1','2','3','4','5','6','7','32','9','A','B','C','D','E','F' }; //ça évite d'avoir 16 if/else
    for (uint64_t i = 0; i <= a; i++) { //on commence du début car little endian
        uint32_t visualisation = (uint32_t)from_.Value[i];
        returned[i * 2 + 2] = constants[visualisation / 16]; //prends les 4 bits du haut (1010 0101 -> 1010)
        returned[i * 2 + 3] = constants[visualisation & 15]; //prends les 4 bits du bas (1010 0101 -> 0101) (grâce au & et parce que 15 c'est 0000 1111)
    }
    returned[from_.length * 2 + 2] = '\0';

    return returned;
}

/* ----------------------------------------------- */
char* number_to_base10_str(const Number from_) {
    Number copy = Create_Number(from_.length); //copy from_ to not destroy the input
    memcpy(copy.Value, from_.Value, from_.length * 4);
    uint64_t number_of_zeros = 0;
    for (uint64_t i = 0; copy.Value[i] == 0; i++) {
        number_of_zeros++; //permet de ne pas avoir à allouer trop de mémoire 
        //(normalement, c'est pas trop long puisque mes fonctions optimise les Numbers)
    }
    uint64_t char_len = from_.length - number_of_zeros;
    //changed to uint32* so log(2^32) = 32log(2) ~= 9,6329 so we ceil at 9,633
    char_len = (char_len * 9633) / 1000 + 2;
    char* char_returned = (char*)calloc(char_len, sizeof(char));
    if (char_returned == NULL) {
        PanickError("char_returned calloc failed (number_to_base10_str)");
        exit(0x10001);
    }
    uint64_t truc = 0;
    uint32_t reste = 0;
    uint64_t pointer = 0;
    char contant[10] = { '0','1','2','3','4','5','6','7','8','9' };


    while (number_of_zeros != copy.length) {
        for (uint64_t i = number_of_zeros; i < copy.length; i++) {
			truc *= 4294967296; //2^32
            truc += copy.Value[i];
            copy.Value[i] = truc / 1000000000;
            if (i == number_of_zeros && copy.Value[i] == 0) {
                number_of_zeros++;
            }
            truc = truc % 1000000000;
        }
		for (int j = 0; j < 9; j++) {
			char_returned[char_len - pointer - 1] = contant[truc%10]; //quand on arrive à la fin, on a le reste qui est entre 0 et 10
			truc = truc / 10;
			pointer++;
		}
        truc = 0;
    }
	FreeNumber(copy); //free copy
    //on va compter le nombre de '0' inutile dans char_returned (ceux à gauche)
    number_of_zeros = 0;
    for (uint64_t i = 0; char_returned[i] == 0 || char_returned[i] == '0'; i++) {
        number_of_zeros++;
    }
    if (from_.sign == true) {
		//si le nombre est négatif, on rajoute le '-' au début
        memmove(char_returned + 1, char_returned + number_of_zeros, char_len - number_of_zeros);
        char_returned = (char*)realloc(char_returned, char_len - number_of_zeros + 1);
        if (char_returned == NULL) {
            PanickError("char_returned realloc failed (number_to_base10_str)");
            exit(0x10001);
        }
		char_returned[0] = '-';
    }
    else {
        for (int i = 0; i < char_len - number_of_zeros; i++) {
            char_returned[i] = char_returned[i + number_of_zeros];
        }
        char_returned = (char*)realloc(char_returned, char_len - number_of_zeros);
	    if (char_returned == NULL) {
		    PanickError("char_returned realloc failed (number_to_base10_str)");
		    exit(0x10001);
	    }
	}
    char_returned[char_len - number_of_zeros] = '\0';
    return char_returned;
}

/* ----------------------------------------------- */

uint64_t* convert_str_to_u64(char* _Input, uint64_t* length) {
    //converts str to uint64_t* that contains numbers from 0 to 999 999 999 for less calculations 
	//returns NULL if _Input is not a string of digits (as well)
    //put the length of the uint64_t* in length

    //each uint64_t will be at most 9 digits long
    //it means, if a number is at most 9 digits, the conversion can be done easily ; else it will be the division by 2^32 strat

    //Anyway
	uint64_t* r = (uint64_t*)calloc(strlen(_Input) / 9 + 1, sizeof(uint64_t));
    if (r == NULL) {
		PanickError("Allocation of r in function convert_str_to_u64 failed");
		exit(0x10001); //failed allocation
    }
	int _Input_length = strlen(_Input);
	for (int i = 0; i < _Input_length%9; i++) {
		if (_Input[i] > '9' || _Input[i] < '0') {
            free(r);
            return NULL;
		}
		r[0] *= 10; r[0] += _Input[i] - '0';

	}
    uint64_t pointer = 1;
    for (int i = _Input_length % 9; i < _Input_length; i += 9) {
        //now we'll do each other number
        for (int j = 0; j < 9; j++) {
            if (_Input[i + j] > '9' || _Input[i + j] < '0') {
                free(r);
                return NULL;
            }
            r[pointer] *= 10; r[pointer] += _Input[i + j] - '0';
        }
        pointer++;
    }
	*(length) = _Input_length/9+ 1;
    return r;

}

uint64_t divide_string_by_2power32(uint64_t* input, uint64_t* number_of_zeros, const uint64_t length) {
    //input has to be from conver_str_to_u64 or from this function itself
    //this returns the rest of the division by 2^32
    //because numbers in input can *only* be 9 digits long, they'll always be less then 2^32 ;
    uint64_t division, rest = 0;
	for (uint64_t i = (*number_of_zeros); i < length; i++) {
		rest *= 1000000000; //10^9
        rest += input[i]; //10^9*rest + input[i]
        division = rest / 4294967296;
        rest %= 4294967296;
		input[i] = division;
		if (i == (*number_of_zeros) && input[i] == 0) {
			(*number_of_zeros)++;
		}
	}
    return rest;
}

Number parse_base10_str(const char* Input)
{
    uint64_t length_ = strlen(Input);
    char* final_variable = (char*)calloc(length_ + 1, 1);
    if (final_variable == NULL) {
        PanickError("Allocation of final_variable in function parse_base10_str failed");
        printf("Ending programm...\n");
        exit(0x10001); //failed allocation
    }

    memcpy(final_variable, Input, length_); //strcpy seems to have issues
    

    uint64_t number_of_zeros = 0; //counts zeros from the left
	uint64_t Number_length = (length_ * 1000) / 9633 + 1; //length of r
    bool sign = false;
    if (final_variable[0] == '-') {
        sign = true;
        final_variable[0] = '0';
        number_of_zeros++;
    }

	uint64_t* new_input = convert_str_to_u64(final_variable, &length_);
    if (new_input == NULL) {
        PanickError("string given to parse_base10_str hasn't right format");
        return { NULL,0, false }; //returns empty Number
    }
	Number r = Create_Number(Number_length); //creating Number
    if (r.Value == NULL) {
		//checking if allocation worked
		PanickError("Allocation of r in function parse_base10_str failed");
		printf("Ending programm...\n");
		exit(0x10001);
    }
	r.sign = sign;

    uint64_t pointer = 1;

	while (number_of_zeros != length_) {
		uint64_t rest = divide_string_by_2power32(new_input, &number_of_zeros, length_);
		r.Value[r.length - pointer] = rest;
		pointer++;
        //easy and clean, just how everyone likes it
	}
	//removes useless 0s at the left of r
	Check_for_useless_zero(&r);
	free(final_variable);
	free(new_input);
	return r;
}

Number parse_base10_str(const char* Input, uint64_t length__)
{   
    uint64_t length_ = length__;
    char* final_variable = (char*)calloc(length_ + 1, 1);
    if (final_variable == NULL) {
        PanickError("Allocation of final_variable in function parse_base10_str failed");
        printf("Ending programm...\n");
        exit(0x10001); //failed allocation
    }

    memcpy(final_variable, Input, length_); //strcpy seems to have issues


    uint64_t number_of_zeros = 0; //counts zeros from the left
    uint64_t Number_length = (length_ * 1000) / 9633 + 1; //length of r
    bool sign = false;
    if (final_variable[0] == '-') {
        sign = true;
        final_variable[0] = '0';
        number_of_zeros++;
    }

    uint64_t* new_input = convert_str_to_u64(final_variable, &length_);
    if (new_input == NULL) {
        PanickError("string given to parse_base10_str hasn't right format");
        return { NULL,0, false }; //returns empty Number
    }
    Number r = Create_Number(Number_length); //creating Number
    if (r.Value == NULL) {
        //checking if allocation worked
        PanickError("Allocation of r in function parse_base10_str failed");
        printf("Ending programm...\n");
        exit(0x10001);
    }
    r.sign = sign;

    uint64_t pointer = 1;

    while (number_of_zeros != length_) {
        uint64_t rest = divide_string_by_2power32(new_input, &number_of_zeros, length_);
        r.Value[r.length - pointer] = rest;
        pointer++;
        //easy and clean, just how everyone likes it
    }
    //removes useless 0s at the left of r
    Check_for_useless_zero(&r);
    free(final_variable);
    free(new_input);
    return r;
}

/* ----------------------------------------------- */

/* TEST */

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
uint64_t some_constant = 75000; //sets a minimum (might be reworked after tests)

Number test_opti_parsing(char* a, uint64_t length) {
    //length is length of a
    //doesn't reset the input
    if (length < some_constant) {
        return parse_base10_str(a,length); //[Something to rework : a will probably be considered as full char* (-> use of strlen) which might create errors]**
        //** to rework this : might do a new type of a char* with a length
    }
    uint64_t split = length / 2;
    char* a1 = a;
    char* a2 = a + split;
    uint64_t a1_length = split;
    uint64_t a2_length = (length + 1) / 2;
    Number first_half = test_opti_parsing(a1, a1_length);
    Number second_half = test_opti_parsing(a2, a2_length);

    //do the 10^split
    Number final = Create_Number(1, 1);
    Number intermidiate = Create_Number(1, 10);
    split++;
    while (split > 0) {
        if (split % 2 == 1) {
            final = multiplication_Numbers(final, intermidiate);
        }
        split = split >> 1; //split /= 2
        intermidiate = multiplication_Numbers(intermidiate, intermidiate); //intermidiate = intermidiate²
    }
    first_half = multiplication_Numbers(first_half, final);
    Number r = addition_Numbers(first_half, second_half);
    FreeNumber(first_half);
    FreeNumber(second_half);
    FreeNumber(final);
    FreeNumber(intermidiate);
    return r;
}


/* END TEST*/

void massive_test(uint64_t from, uint64_t number_) {
    char* a = (char*)"1999999999973237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323299112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173212173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217321437795444333232267232064032323221321636247693232114904247290516526023211256200695622424124436324213203237722261570591329955603235220915325247153520522327230236039067143227109044751334732955434432303205355476795969301371123640339316490939019966713931322254677199432927107345623200562321623226122967327063324327260463223763007365322355353249934325953159324029275232790232106659595053327174591332932933733700349557010112632069637333239735929420732965774713332547512309539666640156301320955440579041232532900516626069543792424732701167142942324920432232954643323276553262513201323200671110923223273414947325631254509411579275327116936766532742932060320099552069613322915329242323639732099915300344932339740532029322501319496109153229359201034932734217651702323794323316746545661633247323241342746600555932327230225154099717941494300532732794373234032132519532711233217007750032050273206059632432732015303323323612690310326931553949023203919663162220294632295159349257402942127324360632019553909103200275141313117321033732016003232010456192117707131332321336321294651520246216760056492713232325132321223142172640325259302953242273212296322212265432670754714132973039232931320323259097771140632934932676465194706571964332372056261117790713259255773532734132199532563261327395023214796100932113700527266327326396675250920760325532649079327912044032094511442631594791613132433215705632043049406597494544753233334623969362321632905644232932666332232567132620327234304132526750016326203297932076111320329676793916264162172749259933241233232511760761659142340716641657663699325954707096424597623222974731432064943232232932323203477704635046603077059095742709406424102323257466232692032374323719011202455322641905465793217610921453246623667563235325359645425327966327103260320250990367613605321705515333536743232323300173251297995901041475332514107520954943205446476201479559345559177433235363212017919566532324146622323279327512732769532539309325250554765940056030394493711167747173444160169537127037397309079604032727739624146732394902491217136765114903743355125565171712205529243652413615132976017329023243294562463261129443203203290932760427179202316207150529907255020242246549032660190097179230213697032932657559043054663050964544967414393322423249715327943453293294532432432644325591296501515432923231532556405705327325563263276326650916753243262479634267909039223424077313236424927370417310735025499561902042365492432374329993406321433332532195661114013620194003273319147134326703207732610227744214204595965332707773695412520205659137599455204941011023735346003232139321135964262321452957403203216353225055132329765505577546632451111323232049932457750329322923250766655793639173245320151500113201265324002205979232032600325920692636939006272234371676903256909679505513291906311615060773233605601940573232032116313231220433602273243200060173227565532339330211262244055329152327069177299737111616296323239915731154132266510329446924110164132426719632735977593204671691169304950332633204261131506277123324049232645631437573373170463713607096937335432633257327324465676056603447029047617765320621671114769329756511316314423563274124520111149932523466632750972732920727661540729063229276032329013663723052232295321433226642652260943264453266565601321325673232907223274532732332599151322044420656505133232632323616093256497210400410566755432400066616175322979304332113293253211592325063255017601375316134322675332032595103293215563232357924439093573299650413602190573265443232406015709103603161716725509752929940533259673263319167427359910972047212571656732593220591932132612045331422917209573235432432732459735467376540032976323270970321252722627329342299946132943619623215327420093232010035621194476732563621732959535324507955323613277016913267529132571501260269624732976131575000797070300751907466326514320732432770967326123143230300932614327110934425525322070322324321325313932357269674722322409604753637793950603500932664224292637147923292563555500732323796132513292375659913225940727234747494223262621267116246301276132232574591562022103321323710196727754940732593031171232777493249363213176641491922102203396079537323203255632632414305521666321329644294266303265301543217509701713596932166155422321353242140319523637553270732344331324233732495432117342954597137909451113697732624259656332559323256743210912322014390361403432452933215625662703224905664732992240546210341367451795006146430367332113573226770235194932064710370360359533214316332730017104737329236676323239356359113040025603200261455907650761454276532431577973247613232237056944669032232613543590713253601326670143320549779746393247623402732761049400241000661019764596091440195263241161753245217964520143253315593352323141096293232031665293250690156326007431345026610324012232054306969514779732152964132004403203732253671702911325990532524039670931323732412665323232920643295232074464679217929959255644932251115736321661179731694367534417626709339545149944720224044293232077221663233250452150727541463327327619329326703999996060427319391325207753233269113220363296659109326325532213261632416312011203797129932910513234073494969074332963241432296355603153545133402332632329295225563276320071344932795369327394079631775523519325429504503262532224553376707694561679406322155402253293403043213230093232755522993232375675032122915419014153243275072944304315413426115329037052553712352742549726923259747722353123250534614174961730513624353239911903241167536170090609606320620132733206366296299962013294607320497535526375327995753";//input, put any number you want (base 10)
    char* b = (char*)"530076667076000964011329323245327256901577969676597266941653276232369920463210641274914372470321031193032324310923132763269532327624090115324524157632321650251710969207014132353246753290326773491457372944954420653214133232944299417321132160325360332944323049362663232320559322633901636703325796075593234721325032112446072914637755546746326469712524324132059015322231324645432025546012551352340596756211320657705325649617315326444325445627162276241331701219461532563693560324305505577323446323197304593790359653201500120354332033966275954365593077532032325232032151325557110665652903203953205591350053205090269320325391797202622329650922332601393321353327232346013232232255232452555332532046679291071541753247366127111343269626305212102603362474320651499725164493132760706033757639032275170104253245505263320305940293323252779043329541022216323277752327103323195031954937650495733253277671326497396732132320911524353263231757323913232632323239973295336354221294533960633260036432069222537003991420099321492362139463247332540441320327615153279313292017741617354443193019641211323039426547014973242426322634119011013765756432202963221704323717632132107329654465244242136323213553320700197602142321932544432935579632453233633219322135732690324454236764703393212327126656777672717347660613291260136357607715007136957372006932032567503260717325090120354204923793100617427997712326997473232032220603272410532921549323274032432321932323251932915767125216372324542532477932796627017599063324451792413263203557546233263604322932553130203532632321593145720794076151694120274253406331220320332320754609094120732697139623232506322500220033232532065744713016232546631009093752970691332121402212323215355554632369203967740321005240161334126172244904639991465771714260253169322972709799002916679573249479732323232074251129363462076493759766353232576010227441245342005329503329131920533541232599696054172431151363255043202252043976645444963272707909236501532033255132141326799427054736347532035732202322322674322794913437910463249939327326315324591953377501302432364164094144192432173593232505922262324972323993293732796343232563325232632996421531532550920796115446270972275416294069136039621663232013233253297632632529463172243210004530323732206222323246320015327325927172033426432103209909732222929361432371402013440322402332632543253950421370975643223322425452132322732320973632654415593260526066327321561603232799223326352547332532503296047003232143706321632560401641939143450012732004253716719770735650422040019036279533327732372324426465532323322946646650714326671370147644327047432675323731632655057002907606323606704290445715905764473332165276397324369337732216773250477736030324122599244204550331033229220326009127675123232257413417673075932326011723235502511414941622631595704732132137107461392123434594674321571332323201324047453299112200040957305757529247507325065663259310097223299443032149323022663333256532792390221326932113231324991264513243269003255936102224999327432126237532132213245160523279453247730053232557936235332675117639496067659423263402902291279663217239534736413323209723277055294912121326190321534642017462310974134907699262430567652333662636252926900769610619324351924033717432307192776032793359305121906957577236215009323232324676316653912932734947203754390332663263230022933299919312779577422316095326329323270326079249913736226260463793003356633326132751232573204672295293659356973016533313267320407620570969291614750932415214763296924579435405632325029666775049425010129932913201202717324092329919177976705302709155045631777763257100534329294553310143929073520503069141173221570125936566616433057350532265169192464636923366909096103232732707031421653390201962223269263254670094350743255322376307215324643513730632126011621649376430956054035303710632943452577656334454491323953756016513277021569664329301113323273297325232330921636962665771546595620433629235343217173496665131995639325713725446461027323272966943259039600513259407667215032514173220913433214932924549099996326326331302067303215300745147766440132769432917924032444193264320572763966332323325430153232335123223225325653265013253297321993213032165503265421654309335252634341332932032323912992969131324433243250367745703250423209200153397924326232670320391259914320523649323050632563213243053237057952630214104293671553253923216720449321333203370376265221446947144990356329207703204593202422917457053220260414513247055776201213765929320103132062654660103710626393903901445640505160323324636742329415465537742206653212231723206316302554326600433140795751274294693973229541032329513426316260966735793146230763732073214220545977601191644603676513267310592071774127542503456032521415209309122945737296504135656297963111052041236069432710322933330320253401705479940009601450245773267302137927246721911216175732610273266061561432913219320963227132732916169550415241353732577566122132250397304343253274443635141725232394660932324126965577269532749324619902171605266373246432161920924134070329732201454206364695606041595501400103969369261264923761734793244224329435325611691440757543276563279062632764632323252169132532093272333032012632799355909355274327329320650563227710923064214150641270250355035326624773243532420597746031774370325291001323272176307162261290925611157309331779297074724953256771914374266631014360647667440001704229169242229320574392665360651327932997252273299250705947495490193253732694264361149732232944360671364732923215102045062032400632473245652750311321639323104361325063270432397463132635734664337905219475563694150357569691449429593203030571324017347361623671277323231762394322320490737320600766325136190425357323322325071606993213232032771503477250542232620307715773213106049717151674961019053154200453253232323532533294499126744576645632276226475527220422624327023222691632173010326516792737633232792505511167132163253591323919243960606471332729031064746460732260037903122226606111364966026743353403594323455732321567732413159932532064940459332949726556360320214363254296325321341270965605676327325325020434325047503253250659744293441653221767922252724501626632632055562167547934371706136742060951325014021994323209037276664114263693299171575033795326533263250322300427534160467323215953322061032329026545732133232917200042657635020019942105155231329750643641550425101454232131465653553227079453260750206341075143224405020634763232610549427513217593241321732003232072045042694560102503194032320756159190193223450662503032323213736067210217032626540545151124200702246050043232517326663955933449257130632932519321057347549456751504705299567324323292697450319143611329545152632512224323295323251322116060353266257952446176330063179327011405321076642732713239619330064233065357350391966593200504332456462072662903223269557072753115931203239379232191410311532211326034053325230406432323974770913959442063332966432326564757573256632042749757773212077733195732000142324293212070073097533417932344737927259757364744659351526350111332744675623332657096675302915327110040314112346943653950614740017422263322901093411304332334732235219732532329271210512532132419660713294221943202657329743669165923257321697953212534232309143210075232721563209263249104591732409616957523610574960302044105191000732463257015349756325672656113275904939914036656733129706713351500093209326126042970706223203245964234032132929761646169320764264323511320943267376131321997039275022739977329326324255712390455323269445177323221320122732309325379032037979467503460920973144739907470329433235323039167331743999059555614147713732966356002232959705322072393543132549051000693239523275332111732301594420203227603326321653363232327332527073210321479676497249264400524420326591935366506951632352056232432643293232447646144944900077425543239727169734637419373239703215719963217296999173235327273430199052002029421252213234100002002320732500126155099391100323100631266714366927623032326001130114043244563257532329316325121323202516674975036010555123590126513250024651795559326744323362732433264373932102413290602331072162326325613294406327326074176367656052277432451375764097245323563799552232225363635459229727370925394666610040932320460512426329569519146491737115432323264132450532345191619365665107324493032377522595326239632329756593232133209327103262505222071075694529532696473204295093379163003321223295363274477321333233275101633254994332091414930464616506977323396706716299262962233039323279619743207577732329107433292432422393669702217932321132327101694920439532177696326052707732343423033273249632055559101607447324973766232209121655643294029075329659236532932032066722254600756796297193233271366153230799011737477366031327037215532630696051132626671492470923260993221191627532479553232004201073475132261946324154771411194423259595664327320626623250771326772295501437716964142514207432326047619266332032710325011933235222157341793292720922716759130613201126360493201266297423320634732170335575260766994632132722121731432073323927691464720451617562033261023063271662322032663243232323260095666296692675977933095632632320303253144146264432165047759793326242097032153560674932095110900065632993740027005263327635936797965637932329625557926251273273216791937325173232032619533235429372455390254006032214732376111032136674327947912732717132911226303229029051322315355736729724526527704324332323533227323261241659329913532473242700452940053143296327993222619024732140029212932297323194553221620570769717926132477359670620362016364110232076579602963265324690271254131660323203232663203269520066322764446412043532735357134159205011603632532403232321231990441367230063245629764973295943219003924395654273205402935417646225317006123157552711021433211575740703527744443323199024373094750513255357967455325220129770193045733275373239044609191063666206417757124140544323244721762017139694017095325413279632732714690957396399322632300403294666721965343303230062134794632660547322343233369107137326506663232465326325795623732603632633232555654137661232545932932023621299910903273954125759532693714596241604950949717979323232795166749332932949490326400927791906473277245519560324232612151945560904233291340620744533233230551737112326161907032710932279796602327432327557254610132329263273327373263232947332253311132251074077274904333223363909431916766534647324232133201219527630566656997256115632326321236512179716144192329192714026325914009651323251441639261763264645649452673273773202931411672324512190432503912933459632704232324413274767165209573651132949959569273261950956975320930141503773725245045371374934293323370935107777377115532906325164234323105345093296452349555301311726611320059650325030375093263232665500932572645143209153712609917376930161460647766745342713603164161570320141766732453279014312432532177977011326923351327152932033421166329319324232210613299320017320535205599514329755743223252433230327230322432327320065262320563294559775379033260120320032320532761596165325032556714506343221041513299442960052230662715206293321130757614932633242252266151977146534771932734625605632552466532273260743255196049331152032012449929764640197013227444457914932327171450070455473265056132666033250167323040325409710999100992216351743245912149340405943243252574961259320323232132029121612505739396593500295324626099190632063494232227200693276647472143234723143153132320619196633279327466073453292332240614766054232693974412193243577632412475132366632136955675025026043209150371379675973113729077213262352306201732923222250443255321335415976932629395430374323214557639394313021696905049732566927446710034473770350909599733612232432632763232603223210306775133732601403533245653623101014213233259457075161493043231230470321524094457452095452734959409733402150321927232039326099553263720313744565993992647432705632050032323079595246332463234521613272329132771703326169399443215557500245323069653277543053346173262592032257476590122616267137196432220327324519144532222464394327590592291140113261632563245322304623320135350324795751413350220309299305032037570266559373732623919069710060332564043776533323229229332799053240232229536324672032504221326992407023293232704220076219709557941032553230954799940250602224042303222099320990645041617719325132723347537596637541795705517527311595054406046151153593295044145955232329172325545103219065265623310932671043299460363102672047037553942654979053273213241032077553327023324323407576452955692267326449526910675232321210517639702160740032735326329320736473225055010570457439040009320327632442055774795323323220643277272545747115373276759122614307322430723432226721312696050423263232992325075327324573220972376732944690932742953174419321730520471290322217320150032150626946332335136932116325323250579499749250114976994777503263732706436501960113420326646357561549674793956399326470943246432219146326341032303232065576217366970923794604367055913932590660940979159976326452243005729632630451267321150532064717761795732535132090626505332324774799596406520661513219132590675145597703223232323326005532166532167132174932570446741073920721105674326716324241791233432723325663232042157912032393703221674340430203263429193230032332492327217933236561491934321043232567263310521691713125444761732143357279700032332660449020217562921220323297067942420403729504053245103252307323036321632132595250327624179052636369403222321332033449141539991632072907433405035603252413267326525204475324290370047432323002401395979532992799794740049467643572323212432594132970274911917557263290177772532022260327129733202323165421327324922034541356960735705322479327327090632599621192514420632574276233632712193252303216347216656699532411912792450910322016332040409327310902672326240332220613275940243797220730139626205377423242426973294955740293793010322323032636367422320035020420354900312550932219325324253225932133204593235726541707543270327934007364375099424493259332057596632209269991461321515055224332613553320099492393434463162797212956444523210323296232725591424549432644125323476542307323003232632565931465456732441332133243225535709470347447329003532632293324369443412562733023272316072350164116032323267933232312250714324007133299223244073946101011675320793267019012912566043277121554696222993235159322923231326132369274692975419732729990659403942234731355952526231307676426643295403206726413263296594030112103703432732763543261932321139322165469116563231437321271375232462322353156925272425269673292429933293710043267347639799525321321251525732900202323294759402760323332197167329329549566321004299393714314632529769423921320732432116626650320279955691626321303253420457690790132206452649412611933213263232462639132666974732630220741694260932202326321932991435460133242555619465370267052102073292432779176600326903213163246353700214325323332673214573209032062762321573031509932532336559212327325201323397649511336436004044373532314559323663326229617126132042244120132397006496414965253230112754797112323253224671145205325320033323413935262919742303362025057527103713230573542771339541249331723054149773417611979529221194323999255565773253232499233777212532390322005439290046321695753326122157639217321195475623610677069103023340327711254329206363216294254673216726650172292143296633232325326379463716663307294463329465963474543230104393329073250469609100237326325220231130232101323122745432432556132329524464399472950229323334541625233263744100620790255471129456110332593254760325121766932045296615260147406723235120973560900341753227543203469244327332350795205567326729175932193241320132516756247473263259259325232625323267932031409957447114591325145671403253376707694561679406322155402253293403043213230093232755522993232375675032122915419014153243275072944304315413426115329037052553712352742549726923259747722353123250534614174961730513624353239911903241167536170090609606320620132733206366296299962013294607320497535526375327995753";
    Number a_conv = parse_base10_str(a);
    Number b_conv = parse_base10_str(b);
    minimum_for_karatsuba = from;
    Number d_conv;
    timespec my_time = { 0, 0 };
    timespec new_time = { 0, 0 };
    long final_time;
    long final_time_2;
    FILE* F = fopen("massive_text.txt", "a+");
    if (F == NULL) {
        PanickError("file allocation failed, ending programm...");
        exit(0x10002);
    }
    for (int i = 0; i < number_; i++) {
        printf("number %u over %u\n", i + 1, number_);
        timespec_get(&my_time, TIME_UTC);
        d_conv = multiplication_Numbers(a_conv, b_conv);
        timespec_get(&new_time, TIME_UTC);
        final_time = new_time.tv_sec - my_time.tv_sec;
        final_time_2 = new_time.tv_nsec - my_time.tv_nsec;
        if (final_time_2 < 0) {
            final_time -= 1;
            final_time_2 += 1000000000;
        }
        fprintf(F, "{%ld.%09ld}\n", final_time, final_time_2);
    }

}

long* timespec_treat(timespec* time)
{
	long* r = (long*)malloc(sizeof(long) * 2);
	if (r == NULL) {
		PanickError("memory allocation failed, ending programm...");
		exit(0x10002);
	}
	r[0] = time[1].tv_sec - time[0].tv_sec;
	r[1] = time[1].tv_nsec-time[0].tv_nsec;
    if (r[1] < 0) {
        r[0] -= 1;
        r[1] += 1000000000;
    }
	return r;
}

int GetButtonPress(void)
{
    return getchar();
}

//test 
/*
int main() {
    if (CConsole) CreateConsoleWindow();
    char* part = (char*)"1999999999973237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323299112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173212173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217321437795444333232267232064032323221321636247693232114904247290516526023211256200695622424124436324213203237722261570591329955603235220915325247153520522327230236039067143227109044751334732955434432303205355476795969301371123640339316490939019966713931322254677199432927107345623200562321623226122967327063324327260463223763007365322355353249934325953159324029275232790232106659595053327174591332932933733700349557010112632069637333239735929420732965774713332547512309539666640156301320955440579041232532900516626069543792424732701167142942324920432232954643323276553262513201323200671110923223273414947325631254509411579275327116936766532742932060320099552069613322915329242323639732099915300344932339740532029322501319496109153229359201034932734217651702323794323316746545661633247323241342746600555932327230225154099717941494300532732794373234032132519532711233217007750032050273206059632432732015303323323612690310326931553949023203919663162220294632295159349257402942127324360632019553909103200275141313117321033732016003232010456192117707131332321336321294651520246216760056492713232325132321223142172640325259302953242273212296322212265432670754714132973039232931320323259097771140632934932676465194706571964332372056261117790713259255773532734132199532563261327395023214796100932113700527266327326396675250920760325532649079327912044032094511442631594791613132433215705632043049406597494544753233334623969362321632905644232932666332232567132620327234304132526750016326203297932076111320329676793916264162172749259933241233232511760761659142340716641657663699325954707096424597623222974731432064943232232932323203477704635046603077059095742709406424102323257466232692032374323719011202455322641905465793217610921453246623667563235325359645425327966327103260320250990367613605321705515333536743232323300173251297995901041475332514107520954943205446476201479559345559177433235363212017919566532324146622323279327512732769532539309325250554765940056030394493711167747173444160169537127037397309079604032727739624146732394902491217136765114903743355125565171712205529243652413615132976017329023243294562463261129443203203290932760427179202316207150529907255020242246549032660190097179230213697032932657559043054663050964544967414393322423249715327943453293294532432432644325591296501515432923231532556405705327325563263276326650916753243262479634267909039223424077313236424927370417310735025499561902042365492432374329993406321433332532195661114013620194003273319147134326703207732610227744214204595965332707773695412520205659137599455204941011023735346003232139321135964262321452957403203216353225055132329765505577546632451111323232049932457750329322923250766655793639173245320151500113201265324002205979232032600325920692636939006272234371676903256909679505513291906311615060773233605601940573232032116313231220433602273243200060173227565532339330211262244055329152327069177299737111616296323239915731154132266510329446924110164132426719632735977593204671691169304950332633204261131506277123324049232645631437573373170463713607096937335432633257327324465676056603447029047617765320621671114769329756511316314423563274124520111149932523466632750972732920727661540729063229276032329013663723052232295321433226642652260943264453266565601321325673232907223274532732332599151322044420656505133232632323616093256497210400410566755432400066616175322979304332113293253211592325063255017601375316134322675332032595103293215563232357924439093573299650413602190573265443232406015709103603161716725509752929940533259673263319167427359910972047212571656732593220591932132612045331422917209573235432432732459735467376540032976323270970321252722627329342299946132943619623215327420093232010035621194476732563621732959535324507955323613277016913267529132571501260269624732976131575000797070300751907466326514320732432770967326123143230300932614327110934425525322070322324321325313932357269674722322409604753637793950603500932664224292637147923292563555500732323796132513292375659913225940727234747494223262621267116246301276132232574591562022103321323710196727754940732593031171232777493249363213176641491922102203396079537323203255632632414305521666321329644294266303265301543217509701713596932166155422321353242140319523637553270732344331324233732495432117342954597137909451113697732624259656332559323256743210912322014390361403432452933215625662703224905664732992240546210341367451795006146430367332113573226770235194932064710370360359533214316332730017104737329236676323239356359113040025603200261455907650761454276532431577973247613232237056944669032232613543590713253601326670143320549779746393247623402732761049400241000661019764596091440195263241161753245217964520143253315593352323141096293232031665293250690156326007431345026610324012232054306969514779732152964132004403203732253671702911325990532524039670931323732412665323232920643295232074464679217929959255644932251115736321661179731694367534417626709339545149944720224044293232077221663233250452150727541463327327619329326703999996060427319391325207753233269113220363296659109326325532213261632416312011203797129932910513234073494969074332963241432296355603153545133402332632329295225563276320071344932795369327394079631775523519325429504503262532224553376707694561679406322155402253293403043213230093232755522993232375675032122915419014153243275072944304315413426115329037052553712352742549726923259747722353123250534614174961730513624353239911903241167536170090609606320620132733206366296299962013294607320497535526375327995753";//input, put any number you want (base 10)
    char* part_= (char*)"5300766670760009640113293232453272569015779696765972669416532762323699204632106412749143724703210311930323243109231327632695323276240901153245241576323216502517109692070141323532467532903267734914573729449544206532141332329442994173211321603253603329443230493626632323205593226339016367033257960755932347213250321124460729146377555467463264697125243241320590153222313246454320255460125513523405967562113206577053256496173153264443254456271622762413317012194615325636935603243055055773234463231973045937903596532015001203543320339662759543655930775320323252320321513255571106656529032039532055913500532050902693203253917972026223296509223326013933213533272323460132322322552324525553325320466792910715417532473661271113432696263052121026033624743206514997251644931327607060337576390322751701042532455052633203059402933232527790433295410222163232777523271033231950319549376504957332532776713264973967321323209115243532632317573239132326323232399732953363542212945339606332600364320692225370039914200993214923621394632473325404413203276151532793132920177416173544431930196412113230394265470149732424263226341190110137657564322029632217043237176321321073296544652442421363232135533207001976021423219325444329355796324532336332193221357326903244542367647033932123271266567776727173476606132912601363576077150071369573720069320325675032607173250901203542049237931006174279977123269974732320322206032724105329215493232740324323219323232519329157671252163723245425324779327966270175990633244517924132632035575462332636043229325531302035326323215931457207940761516941202742534063312203203323207546090941207326971396232325063225002200332325320657447130162325466310090937529706913321214022123232153555546323692039677403210052401613341261722449046399914657717142602531693229727097990029166795732494797323232320742511293634620764937597663532325760102274412453420053295033291319205335412325996960541724311513632550432022520439766454449632727079092365015320332551321413267994270547363475320357322023223226743227949134379104632499393273263153245919533775013024323641640941441924321735932325059222623249723239932937327963432325633252326329964215315325509207961154462709722754162940691360396216632320132332532976326325294631722432100045303237322062223232463200153273259271720334264321032099097322229293614323714020134403224023326325432539504213709756432233224254521323227323209736326544155932605260663273215616032327992233263525473325325032960470032321437063216325604016419391434500127320042537167197707356504220400190362795333277323723244264655323233229466466507143266713701476443270474326753237316326550570029076063236067042904457159057644733321652763973243693377322167732504777360303241225992442045503310332292203260091276751232322574134176730759323260117232355025114149416226315957047321321371074613921234345946743215713323232013240474532991122000409573057575292475073250656632593100972232994430321493230226633332565327923902213269321132313249912645132432690032559361022249993274321262375321322132451605232794532477300532325579362353326751176394960676594232634029022912796632172395347364133232097232770552949121213261903215346420174623109741349076992624305676523336626362529269007696106193243519240337174323071927760327933593051219069575772362150093232323246763166539129327349472037543903326632632300229332999193127795774223160953263293232703260792499137362262604637930033566333261327512325732046722952936593569730165333132673204076205709692916147509324152147632969245794354056323250296667750494250101299329132012027173240923299191779767053027091550456317777632571005343292945533101439290735205030691411732215701259365666164330573505322651691924646369233669090961032327327070314216533902019622232692632546700943507432553223763072153246435137306321260116216493764309560540353037106329434525776563344544913239537560165132770215696643293011133232732973252323309216369626657715465956204336292353432171734966651319956393257137254464610273232729669432590396005132594076672150325141732209134332149329245490999963263263313020673032153007451477664401327694329179240324441932643205727639663323233254301532323351232232253256532650132532973219932130321655032654216543093352526343413329320323239129929691313244332432503677457032504232092001533979243262326703203912599143205236493230506325632132430532370579526302141042936715532539232167204493213332033703762652214469471449903563292077032045932024229174570532202604145132470557762012137659293201031320626546601037106263939039014456405051603233246367423294154655377422066532122317232063163025543266004331407957512742946939732295410323295134263162609667357931462307637320732142205459776011916446036765132673105920717741275425034560325214152093091229457372965041356562979631110520412360694327103229333303202534017054799400096014502457732673021379272467219112161757326102732660615614329132193209632271327329161695504152413537325775661221322503973043432532744436351417252323946609323241269655772695327493246199021716052663732464321619209241340703297322014542063646956060415955014001039693692612649237617347932442243294353256116914407575432765632790626327646323232521691325320932723330320126327993559093552743273293206505632277109230642141506412702503550353266247732435324205977460317743703252910013232721763071622612909256111573093317792970747249532567719143742666310143606476674400017042291692422293205743926653606513279329972522732992507059474954901932537326942643611497322329443606713647329232151020450620324006324732456527503113216393231043613250632704323974631326357346643379052194755636941503575696914494295932030305713240173473616236712773232317623943223204907373206007663251361904253573233223250716069932132320327715034772505422326203077157732131060497171516749610190531542004532532323235325332944991267445766456322762264755272204226243270232226916321730103265167927376332327925055111671321632535913239192439606064713327290310647464607322600379031222266061113649660267433534035943234557323215677324131599325320649404593329497265563603202143632542963253213412709656056763273253250204343250475032532506597442934416532217679222527245016266326320555621675479343717061367420609513250140219943232090372766641142636932991715750337953265332632503223004275341604673232159533220610323290265457321332329172000426576350200199421051552313297506436415504251014542321314656535532270794532607502063410751432244050206347632326105494275132175932413217320032320720450426945601025031940323207561591901932234506625030323232137360672102170326265405451511242007022460500432325173266639559334492571306329325193210573475494567515047052995673243232926974503191436113295451526325122243232953232513221160603532662579524461763300631793270114053210766427327132396193300642330653573503919665932005043324564620726629032232695570727531159312032393792321914103115322113260340533252304064323239747709139594420633329664323265647575732566320427497577732120777331957320001423242932120700730975334179323447379272597573647446593515263501113327446756233326570966753029153271100403141123469436539506147400174222633229010934113043323347322352197325323292712105125321324196607132942219432026573297436691659232573216979532125342323091432100752327215632092632491045917324096169575236105749603020441051910007324632570153497563256726561132759049399140366567331297067133515000932093261260429707062232032459642340321329297616461693207642643235113209432673761313219970392750227399773293263242557123904553232694451773232213201227323093253790320379794675034609209731447399074703294332353230391673317439990595556141477137329663560022329597053220723935431325490510006932395232753321117323015944202032276033263216533632323273325270732103214796764972492644005244203265919353665069516323520562324326432932324476461449449000774255432397271697346374193732397032157199632172969991732353272734301990520020294212522132341000020023207325001261550993911003231006312667143669276230323260011301140432445632575323293163251213232025166749750360105551235901265132500246517955593267443233627324332643739321024132906023310721623263256132944063273260741763676560522774324513757640972453235637995522322253636354592297273709253946666100409323204605124263295695191464917371154323232641324505323451916193656651073244930323775225953262396323297565932321332093271032625052220710756945295326964732042950933791630033212232953632744773213332332751016332549943320914149304646165069773233967067162992629622330393232796197432075777323291074332924324223936697022179323211323271016949204395321776963260527077323434230332732496320555591016074473249737662322091216556432940290753296592365329320320667222546007567962971932332713661532307990117374773660313270372155326306960511326266714924709232609932211916275324795532320042010734751322619463241547714111944232595956643273206266232507713267722955014377169641425142074323260476192663320327103250119332352221573417932927209227167591306132011263604932012662974233206347321703355752607669946321327221217314320733239276914647204516175620332610230632716623220326632432323232600956662966926759779330956326323203032531441462644321650477597933262420970321535606749320951109000656329937400270052633276359367979656379323296255579262512732732167919373251732320326195332354293724553902540060322147323761110321366743279479127327171329112263032290290513223153557367297245265277043243323235332273232612416593299135324732427004529400531432963279932226190247321400292129322973231945532216205707697179261324773596706203620163641102320765796029632653246902712541316603232032326632032695200663227644464120435327353571341592050116036325324032323212319904413672300632456297649732959432190039243956542732054029354176462253170061231575527110214332115757407035277444433231990243730947505132553579674553252201297701930457332753732390446091910636662064177571241405443232447217620171396940170953254132796327327146909573963993226323004032946667219653433032300621347946326605473223432333691071373265066632324653263257956237326036326332325556541376612325459329320236212999109032739541257595326937145962416049509497179793232327951667493329329494903264009277919064732772455195603242326121519455609042332913406207445332332305517371123261619070327109322797966023274323275572546101323292632733273732632329473322533111322510740772749043332233639094319167665346473242321332012195276305666569972561156323263212365121797161441923291927140263259140096513232514416392617632646456494526732737732029314116723245121904325039129334596327042323244132747671652095736511329499595692732619509569753209301415037737252450453713749342933233709351077773771155329063251642343231053450932964523495553013117266113200596503250303750932632326655009325726451432091537126099173769301614606477667453427136031641615703201417667324532790143124325321779770113269233513271529320334211663293193242322106132993200173205352055995143297557432232524332303272303224323273200652623205632945597753790332601203200323205327615961653250325567145063432210415132994429600522306627152062933211307576149326332422522661519771465347719327346256056325524665322732607432551960493311520320124499297646401970132274444579149323271714500704554732650561326660332501673230403254097109991009922163517432459121493404059432432525749612593203232321320291216125057393965935002953246260991906320634942322272006932766474721432347231431531323206191966332793274660734532923322406147660542326939744121932435776324124751323666321369556750250260432091503713796759731137290772132623523062017329232222504432553213354159769326293954303743232145576393943130216969050497325669274467100344737703509095997336122324326327632326032232103067751337326014035332456536231010142132332594570751614930432312304703215240944574520954527349594097334021503219272320393260995532637203137445659939926474327056320500323230795952463324632345216132723291327717033261693994432155575002453230696532775430533461732625920322574765901226162671371964322203273245191445322224643943275905922911401132616325632453223046233201353503247957514133502203092993050320375702665593737326239190697100603325640437765333232292293327990532402322295363246720325042213269924070232932327042200762197095579410325532309547999402506022240423032220993209906450416177193251327233475375966375417957055175273115950544060461511535932950441459552323291723255451032190652656233109326710432994603631026720470375539426549790532732132410320775533270233243234075764529556922673264495269106752323212105176397021607400327353263293207364732250550105704574390400093203276324420557747953233232206432772725457471153732767591226143073224307234322267213126960504232632329923250753273245732209723767329446909327429531744193217305204712903222173201500321506269463323351369321163253232505794997492501149769947775032637327064365019601134203266463575615496747939563993264709432464322191463263410323032320655762173669709237946043670559139325906609409791599763264522430057296326304512673211505320647177617957325351320906265053323247747995964065206615132191325906751455977032232323233260055321665321671321749325704467410739207211056743267163242417912334327233256632320421579120323937032216743404302032634291932300323324923272179332365614919343210432325672633105216917131254447617321433572797000323326604490202175629212203232970679424204037295040532451032523073230363216321325952503276241790526363694032223213320334491415399916320729074334050356032524132673265252044753242903700474323230024013959795329927997947400494676435723232124325941329702749119175572632901777725320222603271297332023231654213273249220345413569607357053224793273270906325996211925144206325742762336327121932523032163472166566995324119127924509103220163320404093273109026723262403322206132759402437972207301396262053774232424269732949557402937930103223230326363674223200350204203549003125509322193253242532259321332045932357265417075432703279340073643750994244932593320575966322092699914613215150552243326135533200994923934344631627972129564445232103232962327255914245494326441253234765423073230032326325659314654567324413321332432255357094703474473290035326322933243694434125627330232723160723501641160323232679332323122507143240071332992232440739461010116753207932670190129125660432771215546962229932351593229232313261323692746929754197327299906594039422347313559525262313076764266";

    //char a[101] = "1234567890";
    //char b[101] = "12345678901234567890";
    uint64_t size = 100000; //1MiB
    bool skip_str_conversion = true;
    char* a = (char*)calloc(size, sizeof(char));
    char* b = (char*)calloc(size, sizeof(char));
    for (int pointer = 0; pointer < size; pointer += 14155) { //14155 char per memcpy
        memcpy(a + pointer, part, min(14155, size - pointer)); //for no overflow
        memcpy(b + pointer, part_, min(14155, size - pointer));
    }
    b[size - 1] = '\0'; // null termination
    a[size - 1] = '\0'; // null termination
    timespec* total_time = (timespec*)calloc(2, sizeof(timespec));
    timespec* parsing_time = (timespec*)calloc(2, sizeof(timespec));
    timespec* parsing_test_time = (timespec*)calloc(2,sizeof(timespec));

    if (timespec_get(&total_time[0], TIME_UTC) == NULL) {
        exit(61);
    }
    parsing_time[0] = total_time[0];
    Number a_conv = parse_base10_str((const char*)a);
    
    if (timespec_get(&parsing_time[1], TIME_UTC) == NULL) {
        exit(62);
    }
    parsing_test_time[0] = parsing_time[1];
    Number a_conv_2 = test_opti_parsing(a,strlen(a));
    if (timespec_get(&parsing_test_time[1], TIME_UTC) == NULL) {
        exit(63);
    }
    total_time[1] = parsing_test_time[1];

    long* final_total_time = timespec_treat(total_time);
    long* final_parsing_time = timespec_treat(parsing_time);
    long* final_test_parsing_time = timespec_treat(parsing_test_time);
   
    printf("total time          : %ld.%09lds\n", final_total_time[0], final_total_time[1]);
    printf("parsing time        : %ld.%09lds\n", final_parsing_time[0], final_parsing_time[1]);
    printf("test time           : %ld.%09lds\n", final_test_parsing_time[0], final_test_parsing_time[1]);

    printf("To test the Numbers data, here is the providing pointers\nnormal_pointer : %p ;\ntest_pointer : %p", a_conv.Value, a_conv_2.Value);

    printf("press enter to end program\n");
    GetButtonPress(); //wait for key press to end the programm
    return 0;
}
/**/

int main()
{
    if (CConsole) CreateConsoleWindow();
    char* part = (char*)"1999999999973237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323299112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173212173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217323732927932329112173237329279323291121732373292793232911217321437795444333232267232064032323221321636247693232114904247290516526023211256200695622424124436324213203237722261570591329955603235220915325247153520522327230236039067143227109044751334732955434432303205355476795969301371123640339316490939019966713931322254677199432927107345623200562321623226122967327063324327260463223763007365322355353249934325953159324029275232790232106659595053327174591332932933733700349557010112632069637333239735929420732965774713332547512309539666640156301320955440579041232532900516626069543792424732701167142942324920432232954643323276553262513201323200671110923223273414947325631254509411579275327116936766532742932060320099552069613322915329242323639732099915300344932339740532029322501319496109153229359201034932734217651702323794323316746545661633247323241342746600555932327230225154099717941494300532732794373234032132519532711233217007750032050273206059632432732015303323323612690310326931553949023203919663162220294632295159349257402942127324360632019553909103200275141313117321033732016003232010456192117707131332321336321294651520246216760056492713232325132321223142172640325259302953242273212296322212265432670754714132973039232931320323259097771140632934932676465194706571964332372056261117790713259255773532734132199532563261327395023214796100932113700527266327326396675250920760325532649079327912044032094511442631594791613132433215705632043049406597494544753233334623969362321632905644232932666332232567132620327234304132526750016326203297932076111320329676793916264162172749259933241233232511760761659142340716641657663699325954707096424597623222974731432064943232232932323203477704635046603077059095742709406424102323257466232692032374323719011202455322641905465793217610921453246623667563235325359645425327966327103260320250990367613605321705515333536743232323300173251297995901041475332514107520954943205446476201479559345559177433235363212017919566532324146622323279327512732769532539309325250554765940056030394493711167747173444160169537127037397309079604032727739624146732394902491217136765114903743355125565171712205529243652413615132976017329023243294562463261129443203203290932760427179202316207150529907255020242246549032660190097179230213697032932657559043054663050964544967414393322423249715327943453293294532432432644325591296501515432923231532556405705327325563263276326650916753243262479634267909039223424077313236424927370417310735025499561902042365492432374329993406321433332532195661114013620194003273319147134326703207732610227744214204595965332707773695412520205659137599455204941011023735346003232139321135964262321452957403203216353225055132329765505577546632451111323232049932457750329322923250766655793639173245320151500113201265324002205979232032600325920692636939006272234371676903256909679505513291906311615060773233605601940573232032116313231220433602273243200060173227565532339330211262244055329152327069177299737111616296323239915731154132266510329446924110164132426719632735977593204671691169304950332633204261131506277123324049232645631437573373170463713607096937335432633257327324465676056603447029047617765320621671114769329756511316314423563274124520111149932523466632750972732920727661540729063229276032329013663723052232295321433226642652260943264453266565601321325673232907223274532732332599151322044420656505133232632323616093256497210400410566755432400066616175322979304332113293253211592325063255017601375316134322675332032595103293215563232357924439093573299650413602190573265443232406015709103603161716725509752929940533259673263319167427359910972047212571656732593220591932132612045331422917209573235432432732459735467376540032976323270970321252722627329342299946132943619623215327420093232010035621194476732563621732959535324507955323613277016913267529132571501260269624732976131575000797070300751907466326514320732432770967326123143230300932614327110934425525322070322324321325313932357269674722322409604753637793950603500932664224292637147923292563555500732323796132513292375659913225940727234747494223262621267116246301276132232574591562022103321323710196727754940732593031171232777493249363213176641491922102203396079537323203255632632414305521666321329644294266303265301543217509701713596932166155422321353242140319523637553270732344331324233732495432117342954597137909451113697732624259656332559323256743210912322014390361403432452933215625662703224905664732992240546210341367451795006146430367332113573226770235194932064710370360359533214316332730017104737329236676323239356359113040025603200261455907650761454276532431577973247613232237056944669032232613543590713253601326670143320549779746393247623402732761049400241000661019764596091440195263241161753245217964520143253315593352323141096293232031665293250690156326007431345026610324012232054306969514779732152964132004403203732253671702911325990532524039670931323732412665323232920643295232074464679217929959255644932251115736321661179731694367534417626709339545149944720224044293232077221663233250452150727541463327327619329326703999996060427319391325207753233269113220363296659109326325532213261632416312011203797129932910513234073494969074332963241432296355603153545133402332632329295225563276320071344932795369327394079631775523519325429504503262532224553376707694561679406322155402253293403043213230093232755522993232375675032122915419014153243275072944304315413426115329037052553712352742549726923259747722353123250534614174961730513624353239911903241167536170090609606320620132733206366296299962013294607320497535526375327995753";//input, put any number you want (base 10)
    char* part_= (char*)"5300766670760009640113293232453272569015779696765972669416532762323699204632106412749143724703210311930323243109231327632695323276240901153245241576323216502517109692070141323532467532903267734914573729449544206532141332329442994173211321603253603329443230493626632323205593226339016367033257960755932347213250321124460729146377555467463264697125243241320590153222313246454320255460125513523405967562113206577053256496173153264443254456271622762413317012194615325636935603243055055773234463231973045937903596532015001203543320339662759543655930775320323252320321513255571106656529032039532055913500532050902693203253917972026223296509223326013933213533272323460132322322552324525553325320466792910715417532473661271113432696263052121026033624743206514997251644931327607060337576390322751701042532455052633203059402933232527790433295410222163232777523271033231950319549376504957332532776713264973967321323209115243532632317573239132326323232399732953363542212945339606332600364320692225370039914200993214923621394632473325404413203276151532793132920177416173544431930196412113230394265470149732424263226341190110137657564322029632217043237176321321073296544652442421363232135533207001976021423219325444329355796324532336332193221357326903244542367647033932123271266567776727173476606132912601363576077150071369573720069320325675032607173250901203542049237931006174279977123269974732320322206032724105329215493232740324323219323232519329157671252163723245425324779327966270175990633244517924132632035575462332636043229325531302035326323215931457207940761516941202742534063312203203323207546090941207326971396232325063225002200332325320657447130162325466310090937529706913321214022123232153555546323692039677403210052401613341261722449046399914657717142602531693229727097990029166795732494797323232320742511293634620764937597663532325760102274412453420053295033291319205335412325996960541724311513632550432022520439766454449632727079092365015320332551321413267994270547363475320357322023223226743227949134379104632499393273263153245919533775013024323641640941441924321735932325059222623249723239932937327963432325633252326329964215315325509207961154462709722754162940691360396216632320132332532976326325294631722432100045303237322062223232463200153273259271720334264321032099097322229293614323714020134403224023326325432539504213709756432233224254521323227323209736326544155932605260663273215616032327992233263525473325325032960470032321437063216325604016419391434500127320042537167197707356504220400190362795333277323723244264655323233229466466507143266713701476443270474326753237316326550570029076063236067042904457159057644733321652763973243693377322167732504777360303241225992442045503310332292203260091276751232322574134176730759323260117232355025114149416226315957047321321371074613921234345946743215713323232013240474532991122000409573057575292475073250656632593100972232994430321493230226633332565327923902213269321132313249912645132432690032559361022249993274321262375321322132451605232794532477300532325579362353326751176394960676594232634029022912796632172395347364133232097232770552949121213261903215346420174623109741349076992624305676523336626362529269007696106193243519240337174323071927760327933593051219069575772362150093232323246763166539129327349472037543903326632632300229332999193127795774223160953263293232703260792499137362262604637930033566333261327512325732046722952936593569730165333132673204076205709692916147509324152147632969245794354056323250296667750494250101299329132012027173240923299191779767053027091550456317777632571005343292945533101439290735205030691411732215701259365666164330573505322651691924646369233669090961032327327070314216533902019622232692632546700943507432553223763072153246435137306321260116216493764309560540353037106329434525776563344544913239537560165132770215696643293011133232732973252323309216369626657715465956204336292353432171734966651319956393257137254464610273232729669432590396005132594076672150325141732209134332149329245490999963263263313020673032153007451477664401327694329179240324441932643205727639663323233254301532323351232232253256532650132532973219932130321655032654216543093352526343413329320323239129929691313244332432503677457032504232092001533979243262326703203912599143205236493230506325632132430532370579526302141042936715532539232167204493213332033703762652214469471449903563292077032045932024229174570532202604145132470557762012137659293201031320626546601037106263939039014456405051603233246367423294154655377422066532122317232063163025543266004331407957512742946939732295410323295134263162609667357931462307637320732142205459776011916446036765132673105920717741275425034560325214152093091229457372965041356562979631110520412360694327103229333303202534017054799400096014502457732673021379272467219112161757326102732660615614329132193209632271327329161695504152413537325775661221322503973043432532744436351417252323946609323241269655772695327493246199021716052663732464321619209241340703297322014542063646956060415955014001039693692612649237617347932442243294353256116914407575432765632790626327646323232521691325320932723330320126327993559093552743273293206505632277109230642141506412702503550353266247732435324205977460317743703252910013232721763071622612909256111573093317792970747249532567719143742666310143606476674400017042291692422293205743926653606513279329972522732992507059474954901932537326942643611497322329443606713647329232151020450620324006324732456527503113216393231043613250632704323974631326357346643379052194755636941503575696914494295932030305713240173473616236712773232317623943223204907373206007663251361904253573233223250716069932132320327715034772505422326203077157732131060497171516749610190531542004532532323235325332944991267445766456322762264755272204226243270232226916321730103265167927376332327925055111671321632535913239192439606064713327290310647464607322600379031222266061113649660267433534035943234557323215677324131599325320649404593329497265563603202143632542963253213412709656056763273253250204343250475032532506597442934416532217679222527245016266326320555621675479343717061367420609513250140219943232090372766641142636932991715750337953265332632503223004275341604673232159533220610323290265457321332329172000426576350200199421051552313297506436415504251014542321314656535532270794532607502063410751432244050206347632326105494275132175932413217320032320720450426945601025031940323207561591901932234506625030323232137360672102170326265405451511242007022460500432325173266639559334492571306329325193210573475494567515047052995673243232926974503191436113295451526325122243232953232513221160603532662579524461763300631793270114053210766427327132396193300642330653573503919665932005043324564620726629032232695570727531159312032393792321914103115322113260340533252304064323239747709139594420633329664323265647575732566320427497577732120777331957320001423242932120700730975334179323447379272597573647446593515263501113327446756233326570966753029153271100403141123469436539506147400174222633229010934113043323347322352197325323292712105125321324196607132942219432026573297436691659232573216979532125342323091432100752327215632092632491045917324096169575236105749603020441051910007324632570153497563256726561132759049399140366567331297067133515000932093261260429707062232032459642340321329297616461693207642643235113209432673761313219970392750227399773293263242557123904553232694451773232213201227323093253790320379794675034609209731447399074703294332353230391673317439990595556141477137329663560022329597053220723935431325490510006932395232753321117323015944202032276033263216533632323273325270732103214796764972492644005244203265919353665069516323520562324326432932324476461449449000774255432397271697346374193732397032157199632172969991732353272734301990520020294212522132341000020023207325001261550993911003231006312667143669276230323260011301140432445632575323293163251213232025166749750360105551235901265132500246517955593267443233627324332643739321024132906023310721623263256132944063273260741763676560522774324513757640972453235637995522322253636354592297273709253946666100409323204605124263295695191464917371154323232641324505323451916193656651073244930323775225953262396323297565932321332093271032625052220710756945295326964732042950933791630033212232953632744773213332332751016332549943320914149304646165069773233967067162992629622330393232796197432075777323291074332924324223936697022179323211323271016949204395321776963260527077323434230332732496320555591016074473249737662322091216556432940290753296592365329320320667222546007567962971932332713661532307990117374773660313270372155326306960511326266714924709232609932211916275324795532320042010734751322619463241547714111944232595956643273206266232507713267722955014377169641425142074323260476192663320327103250119332352221573417932927209227167591306132011263604932012662974233206347321703355752607669946321327221217314320733239276914647204516175620332610230632716623220326632432323232600956662966926759779330956326323203032531441462644321650477597933262420970321535606749320951109000656329937400270052633276359367979656379323296255579262512732732167919373251732320326195332354293724553902540060322147323761110321366743279479127327171329112263032290290513223153557367297245265277043243323235332273232612416593299135324732427004529400531432963279932226190247321400292129322973231945532216205707697179261324773596706203620163641102320765796029632653246902712541316603232032326632032695200663227644464120435327353571341592050116036325324032323212319904413672300632456297649732959432190039243956542732054029354176462253170061231575527110214332115757407035277444433231990243730947505132553579674553252201297701930457332753732390446091910636662064177571241405443232447217620171396940170953254132796327327146909573963993226323004032946667219653433032300621347946326605473223432333691071373265066632324653263257956237326036326332325556541376612325459329320236212999109032739541257595326937145962416049509497179793232327951667493329329494903264009277919064732772455195603242326121519455609042332913406207445332332305517371123261619070327109322797966023274323275572546101323292632733273732632329473322533111322510740772749043332233639094319167665346473242321332012195276305666569972561156323263212365121797161441923291927140263259140096513232514416392617632646456494526732737732029314116723245121904325039129334596327042323244132747671652095736511329499595692732619509569753209301415037737252450453713749342933233709351077773771155329063251642343231053450932964523495553013117266113200596503250303750932632326655009325726451432091537126099173769301614606477667453427136031641615703201417667324532790143124325321779770113269233513271529320334211663293193242322106132993200173205352055995143297557432232524332303272303224323273200652623205632945597753790332601203200323205327615961653250325567145063432210415132994429600522306627152062933211307576149326332422522661519771465347719327346256056325524665322732607432551960493311520320124499297646401970132274444579149323271714500704554732650561326660332501673230403254097109991009922163517432459121493404059432432525749612593203232321320291216125057393965935002953246260991906320634942322272006932766474721432347231431531323206191966332793274660734532923322406147660542326939744121932435776324124751323666321369556750250260432091503713796759731137290772132623523062017329232222504432553213354159769326293954303743232145576393943130216969050497325669274467100344737703509095997336122324326327632326032232103067751337326014035332456536231010142132332594570751614930432312304703215240944574520954527349594097334021503219272320393260995532637203137445659939926474327056320500323230795952463324632345216132723291327717033261693994432155575002453230696532775430533461732625920322574765901226162671371964322203273245191445322224643943275905922911401132616325632453223046233201353503247957514133502203092993050320375702665593737326239190697100603325640437765333232292293327990532402322295363246720325042213269924070232932327042200762197095579410325532309547999402506022240423032220993209906450416177193251327233475375966375417957055175273115950544060461511535932950441459552323291723255451032190652656233109326710432994603631026720470375539426549790532732132410320775533270233243234075764529556922673264495269106752323212105176397021607400327353263293207364732250550105704574390400093203276324420557747953233232206432772725457471153732767591226143073224307234322267213126960504232632329923250753273245732209723767329446909327429531744193217305204712903222173201500321506269463323351369321163253232505794997492501149769947775032637327064365019601134203266463575615496747939563993264709432464322191463263410323032320655762173669709237946043670559139325906609409791599763264522430057296326304512673211505320647177617957325351320906265053323247747995964065206615132191325906751455977032232323233260055321665321671321749325704467410739207211056743267163242417912334327233256632320421579120323937032216743404302032634291932300323324923272179332365614919343210432325672633105216917131254447617321433572797000323326604490202175629212203232970679424204037295040532451032523073230363216321325952503276241790526363694032223213320334491415399916320729074334050356032524132673265252044753242903700474323230024013959795329927997947400494676435723232124325941329702749119175572632901777725320222603271297332023231654213273249220345413569607357053224793273270906325996211925144206325742762336327121932523032163472166566995324119127924509103220163320404093273109026723262403322206132759402437972207301396262053774232424269732949557402937930103223230326363674223200350204203549003125509322193253242532259321332045932357265417075432703279340073643750994244932593320575966322092699914613215150552243326135533200994923934344631627972129564445232103232962327255914245494326441253234765423073230032326325659314654567324413321332432255357094703474473290035326322933243694434125627330232723160723501641160323232679332323122507143240071332992232440739461010116753207932670190129125660432771215546962229932351593229232313261323692746929754197327299906594039422347313559525262313076764266";

    //char a[101] = "1234567890";
    //char b[101] = "12345678901234567890";
    uint64_t size = 1024*1024; //1MiB
    bool skip_str_conversion = true;
    char* a = (char*)calloc(size, sizeof(char));
    char* b = (char*)calloc(size, sizeof(char));
    for (int pointer = 0; pointer < size; pointer += 14155) { //14155 char per memcpy
        memcpy(a + pointer, part, min(14155, size - pointer)); //for no overflow
        memcpy(b + pointer, part_, min(14155, size - pointer));
    }
    b[size - 1] = '\0'; // null termination
    a[size - 1] = '\0'; // null termination
    timespec* total_time = (timespec*)calloc(2, sizeof(timespec));
    timespec* parsing_time = (timespec*)calloc(2, sizeof(timespec));
    timespec* addition_time = (timespec*)calloc(2, sizeof(timespec));
    timespec* multiplication_time = (timespec*)calloc(2, sizeof(timespec));
    timespec* base10_conversion_time1 = (timespec*)calloc(2, sizeof(timespec));
    timespec* base10_conversion_time2 = (timespec*)calloc(2, sizeof(timespec));
    timespec* base10_conversion_time3 = (timespec*)calloc(2, sizeof(timespec));
    timespec* base10_conversion_time4 = (timespec*)calloc(2, sizeof(timespec));
	char* a_str = NULL; //temp
	char* b_str = NULL; //temp
	char* c_str = NULL; //temp
	char* d_str = NULL; //temp

    timespec* former_multiplication_time = (timespec*)calloc(2, sizeof(timespec));


    if (timespec_get(&total_time[0], TIME_UTC) == NULL) {
        exit(61);
    }
    parsing_time[0] = total_time[0];
    Number a_conv = parse_base10_str((const char*)a);
    Number b_conv = parse_base10_str((const char*)b);
    if (timespec_get(&parsing_time[1], TIME_UTC) == NULL) {
        exit(62);
    }
    addition_time[0] = parsing_time[1];
    Number c_conv = addition_Numbers(a_conv, b_conv);
    if (timespec_get(&addition_time[1], TIME_UTC) == NULL) {
        exit(63);
    }
    multiplication_time[0] = addition_time[1];
    Number d_conv = test_Multiplication_Numbers(a_conv, b_conv);
    if (timespec_get(&multiplication_time[1], TIME_UTC) == NULL) {
        exit(64);
    }
    if (!skip_str_conversion) {
        base10_conversion_time1[0] = multiplication_time[1];
        a_str = number_to_base10_str(a_conv);
        if (timespec_get(&base10_conversion_time1[1], TIME_UTC) == NULL) {
            exit(65);
        }
        base10_conversion_time2[0] = base10_conversion_time1[1];
        b_str = number_to_base10_str(b_conv);
        if (timespec_get(&base10_conversion_time2[1], TIME_UTC) == NULL) {
            exit(66);
        }
        base10_conversion_time3[0] = base10_conversion_time2[1];
        c_str = number_to_base10_str(c_conv);
        if (timespec_get(&base10_conversion_time3[1], TIME_UTC) == NULL) {
            exit(67);
        }
        base10_conversion_time4[0] = base10_conversion_time3[1];
        d_str = number_to_base10_str(d_conv);
    }
    
    if (timespec_get(&total_time[1], TIME_UTC) == NULL) {
        exit(68);
    }
    if (!skip_str_conversion) base10_conversion_time4[1] = total_time[1];


    //calcul du temps que ça à pris (simple soustraction)
    long* final_total_time = timespec_treat(total_time);
    long* final_parsing_time = timespec_treat(parsing_time);
    long* final_addition_time = timespec_treat(addition_time);
    long* final_multiplication_time = timespec_treat(multiplication_time);
    long* final_base10_conversion_time1 = timespec_treat(base10_conversion_time1);
    long* final_base10_conversion_time2 = timespec_treat(base10_conversion_time2);
    long* final_base10_conversion_time3 = timespec_treat(base10_conversion_time3);
    long* final_base10_conversion_time4 = timespec_treat(base10_conversion_time4);
    if (!skip_str_conversion) {
        printf("nombre a base 10 : %s\n", a_str); //will have an actual value
        
        printf("\n");
        printf("nombre b base 10 : %s\n", b_str);
        printf("\n");
        printf("addition a+b : %s\n", c_str);
        printf("\n");
        printf("test of multiplication : %s\n", d_str);
        printf("\n");
        printf("number a pointer : %p ; a_str pointer : %p\n", a_conv.Value, a_str);
        printf("number b pointer : %p ; b_str pointer : %p\n", b_conv.Value, b_str);
        printf("number c pointer : %p ; c_str pointer : %p\n", c_conv.Value, c_str);
        printf("number d pointer : %p ; d_str pointer : %p\n", d_conv.Value, d_str);
    }
    else {
		printf("skipped the display of number, printing the time instead\n");
    }
	printf("total time          : %ld.%09lds\n", final_total_time[0], final_total_time[1]);
	printf("parsing time        : %ld.%09lds\n", final_parsing_time[0], final_parsing_time[1]);
	printf("addition time       : %ld.%09lds\n", final_addition_time[0], final_addition_time[1]);
	printf("multiplication time : %ld.%09lds\n", final_multiplication_time[0], final_multiplication_time[1]);
    if (!skip_str_conversion) {
        printf("base10 conversion time (first a) : %ld.%09lds\n", final_base10_conversion_time1[0], final_base10_conversion_time1[1]);
        printf("base10 conversion time (first b) : %ld.%09lds\n", final_base10_conversion_time2[0], final_base10_conversion_time2[1]);
        printf("base10 conversion time (add a+b) : %ld.%09lds\n", final_base10_conversion_time3[0], final_base10_conversion_time3[1]);
        printf("base10 conversion time (mul a*b) : %ld.%09lds\n", final_base10_conversion_time4[0], final_base10_conversion_time4[1]);
    }
    //massive_test(25, 175);
    printf("press enter to end program\n");
	GetButtonPress(); //wait for key press to end the programm
    return 0;
}