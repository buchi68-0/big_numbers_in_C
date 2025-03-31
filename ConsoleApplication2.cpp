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
    uint8_t* Value;
    uint64_t length;
    bool flag;
};

void FreeNumber(Number _Tofree) {
    free(_Tofree.Value);
    _Tofree.Value = NULL;
    _Tofree.length = 0;
}
//faire un signed number : il faut juste ajouter un flag qui vaut 1 ou 0 suivant s'il est négatif ou pas
/*l'addition/soustraction reste simple :
positif + positif : rien ne change
positif - positif : rien ne change (si le nombre 1 est supérieur au nombre 2 sinon on mets le flag puis on ajoute au lieu de reduire)
positif + négatif = positif - positif
negatif + positif = -(positif - positif)
positif - négatif = positif + positif
negatif - negatif = -(positif - positif)
negatif + negatif = -(positif + positif)*/
//Idée : faire des fonctions cachées pour chaque cas (puisque le négatif peuvent toujour se changer en positif pour faire l'opération)
//bref

void PanickError(const char* msg) {
    printf("\nError : %s",msg);
}
void PanickError(char* msg) {
    printf("\nError : %s\n",msg);
}

Number Create_Number(uint64_t base_length) {
    Number r = { (uint8_t*)calloc(base_length,1), base_length, false };
    if (r.Value == NULL) {
        PanickError("allocation of r in function Create_Number failed");
        printf("Ending programm...\n");
        exit(0x10001); 
    }
    return r;
}


// ----------- UNUSED -----------------
bool Equal(Number Var1, Number Var2) {
    //complexité O(n)
    int64_t difference = Var1.length-Var2.length;
    for (int64_t i = Var2.length; i > 0; i--) {
        if (Var1.Value[i+difference] != Var2.Value[i]) return false;
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

Number Max(Number Var1, Number Var2) {
    //Doesn't care about the flag
    //Is actually used now
    if (Var1.length > Var2.length) {
        uint64_t difference = Var1.length-Var2.length;
        for (uint64_t i = 0; i < difference; i++) {
            if (Var1.Value[i] > 0)return Var1;
        }
        for (uint64_t i = 0; i < Var2.length;i++){
            if (Var1.Value[i+difference] > Var2.Value[i]) return Var1;
            else if (Var1.Value[i+difference] < Var2.Value[i]) return Var2;
        }
    }
    else {
        uint64_t difference = Var2.length-Var1.length;
        for (uint64_t i = 0; i < difference; i++) {
            if (Var2.Value[i] > 0)return Var2;
        }
        for (uint64_t i = 0; i < Var1.length;i++){
            if (Var2.Value[i+difference] > Var1.Value[i]) return Var1;
            else if (Var2.Value[i+difference] < Var1.Value[i]) return Var2;
        }
    }
    return Var2; //Si les deux sont égaux, on en retourne juste 1
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


bool format_str_for_parsing(char* _Input,uint64_t Input_length) {
    //takes an Input (char*) supposed to be a string of digits (i.e : "1234")
    //formats it so it can be used propelly by parse_base10_str (-> {1,2,3,4})
    //returns false if Input not a string of digits ; true else
    for (uint64_t i = 0; i < Input_length; i++) { 
        if ((48 > _Input[i] || _Input[i] > 57)) {
            return false; //ce n'est pas un chiffre; on retourne false
        }
        _Input[i] -= '0'; //sinon on coverti pour avoir {1,2,3,4} par exemple au lieu de {49,50,51,52}
    }
    return true;
}

void divide_char_by2(char* _Input, uint64_t Input_length, uint64_t* Number_of_zero) {
    //"divide" _Input by 2 ; used for parsing
    //won't create errors if _Input hasn't 'format_str_for_parsing' format
    //WILL create errors if _Input[Input_length-1] is even (exception : Input_length != strlen(_Input))
    //add to Number_of_zero if new useless '0' detected

    for (uint64_t i = Input_length-1; i >= (*Number_of_zero) && i < Input_length; i--) { 
        // i >= (*Number_of_zero) && i < Input_length :
        //is a security if (*Number_of_zero) is 0 (then; it will always be >= 0 because it's unsigned)
        if (_Input[i] % 2 == 1) { //if even
            if (i == Input_length - 1) {
                printf("burh"); //trap to debugger
            }
            _Input[i] -= 1; //remove 1
            _Input[i + 1] += 5; //and add 5 to next digit (creates error if i is strlen(_Input-1)
        }
        _Input[i] = _Input[i] / 2; //dividing by 2
        if (i == (*Number_of_zero) && _Input[i] == 0) { //new zero detected (at the left)
            for (uint64_t j = i; j < Input_length && _Input[j] == 0; j++) (*Number_of_zero)++; 
            //add 1 to Number_of_zero (and rechecks if others zeros on the right)
            //it's O(1) after first call else it's O(k) with k the number of useless '0'
        }
    }
}

Number parse_base10_str(char* _Input) {
    //_Input doit être une chaîne de charactères représentant des chiffres
    //parse_base10_str est moins efficace s'il y a des '0' inutiles (à gauche de la châine)
    uint64_t length_ = strlen(_Input);
    char* final_variable = (char*)calloc(length_+1,1);
    if (final_variable == NULL) {
        PanickError("Allocation of final_variable in function parse_base10_str failed");
        printf("Ending programm...\n");
        exit(0x10001); //failed allocation
    }

    memcpy(final_variable,_Input,length_); //strcpy seems to have issues
    
    uint64_t number_of_zeros = 0; //counts zeros from the left
    
    // final_variable verification
    bool verification = format_str_for_parsing(final_variable,length_);
    if (!verification) {
        PanickError("string given to parse_base10_str hasn't right format");
        return {NULL,0}; //returns empty Number
    }
    
    
    uint64_t length = length_-number_of_zeros; //length of r
    
    //on a : 256**x = 10**y ; on a y (length); on cherche x : on a xlog(256) = ylog(10) <==> x = y/log(256) (log(10) = 1; on divise par log(256) de chaque côté)
    length = (length*100)/241 + 1; 

    Number r = Create_Number(length); //creating Number
    if (r.Value == NULL) { //checking if allocation worked
        PanickError("Allocation of r in function parse_base10_str failed");
        printf("Ending programm...\n");
        exit(0x10001); 
    }
    uint8_t current_number = 0; //next value to add to r

    uint64_t pointer = 1; //which value of r the programm will modify (used as r.Value[r.length-pointer])

    int nombre_operation = 0; //track the bit position to modify current_number

    while (final_variable[0] != 10) { //The loop always end (verified)
        if (final_variable[length_ - 1] % 2 == 1) { //handle the last digit (if even)
            final_variable[length_ - 1] -= 1; //if first digit even, remove it and add to current_number
            current_number |= (1 << nombre_operation); //use bit shifting
            //using 'or' operator because it might be faster than an add
        }
        nombre_operation += 1; //each loop iteration, the bit we might change changes
        if (nombre_operation == 8) { //if it's 8, we finished the byte, coming back to zero
            r.Value[r.length-pointer] = current_number; //add to r
            current_number = 0;
            nombre_operation = 0;
            pointer++; //change pointer
        }
        divide_char_by2(final_variable,length_,&number_of_zeros);
        if (number_of_zeros == length_) final_variable[0] = 10;
    }
    r.Value[r.length-pointer] = current_number; 
    //when loop is finished, current_number can still have a value, we have to add it still
    

    //removes useless 0s at the left of r
    int64_t test = 0; //with the optimized size calculation, this thing is about O(3) maximum
    for (uint64_t i = 0; r.Value[i] == 0; i++) {
        test++;
    }
    if (test != 0) { //if more than 1 useless 0
        r.length = r.length - test;
        for (uint64_t i = 0; i < r.length; i++) {
            r.Value[i] = r.Value[i + test]; //O(n) ; we move everything to the left
        }
        r.Value = (uint8_t*)realloc(r.Value, r.length); //we realloc to a Block of right size
        if (r.Value == NULL) { //checking if reallocation worked
            PanickError("Reallocation of r in function parse_base10_str failed");
            printf("Ending programm...\n");
            exit(0x10001); 
        }
    }

    return r;
}

/* ----------------------------------------------- */

//BASIC OPERATIONS
/*
* The only callable functions are subtraction_Numbers and addiction_Numbers
* Others could make false results or even maybe errors
*/
Number _Subtraction_No_Flag(Number _A1, Number _A2) {
    //Child function, DO NOT CALL

    Number Result = Create_Number(_A1.length); //max size : _A1's size
    uint8_t retenue = 0;
    int16_t intermidiate = 0;
    for (int i = 0; i < _A2.length; i++) {
        intermidiate = _A1.Value[_A1.length - i - 1] - _A2.Value[_A2.length - i - 1] - retenue;
        if (intermidiate < 0) {
            retenue = 1;
            intermidiate += 256;
        }
        Result.Value[Result.length - i - 1] = intermidiate;
    }
    for (int i = _A2.length; i < _A1.length; i++) {
        intermidiate = _A1.Value[_A1.length - i - 1] - retenue;
        if (intermidiate < 0) {
            retenue = 1;
            intermidiate += 256;
        }
        Result.Value[Result.length - i - 1] = intermidiate;
    }
    return Result;
}
Number _Addition_No_Flag(Number _A1, Number _A2) {
    //_A1.length HAS to be SUPERIOR or equal to _A2.length
    //Child function DO NOT CALL

    uint64_t length = _A1.length + 1; //length of result
    Number result = Create_Number(length);
    uint16_t _number = 0; //it's the intermediate for the addition
    //this is uint16 because if there's 255+255 it's bigger than 255 so we need one more bit
    //(but uint9_t doesn't exist)
    for (uint64_t i = 0; i < _A2.length; i++) { //we add normally until we reach the end of one Number
        _number += _A1.Value[_A1.length - i - 1] + _A2.Value[_A2.length - i - 1];
        //we add because of the potential retenue
        result.Value[result.length - i - 1] = _number & 255; //takes first 8 bits
        _number = _number >> 8; //shifting to keep the retenue
        //(even though yeah the retenue can only be 1 or 0)
    }
    for (uint64_t i = _A2.length; i < _A1.length; i++) {
        _number += _A1.Value[_A1.length - i - 1];
        result.Value[result.length - i - 1] = _number & 255;
        _number = _number >> 8;
    }
    result.Value[0] = _number;


    //check for useless 0s
    uint64_t retest = 0;
    for (uint64_t i = 0; result.Value[i] == 0; i++) {
        retest++;
    }
    if (retest != 0) {
        result.length -= retest;
        for (uint64_t i = 0; i < result.length; i++) {
            result.Value[i] = result.Value[i + retest];
        }
        result.Value = (uint8_t*)realloc(result.Value, result.length);
        if (result.Value == NULL) { //checking if reallocation worked
            PanickError("Reallocation of result in function addition_Numbers failed");
            printf("Ending programm...\n");
            exit(0x10001);
        }
    }
    return result;
}



Number subtraction_Numbers(Number _Base, Number _Subtraction) {
    //Does _Base - _Subtraction
    //This is the called function
    if (!(_Base.flag || _Subtraction.flag)) { //positive - positive
        Number max__ = Max(_Base,_Subtraction);
        if (max__.Value == _Base.Value) return _Subtraction_No_Flag(_Base,_Subtraction);
        else {
            Number r = _Subtraction_No_Flag(_Subtraction,_Base);
            r.flag = true; // a-b = -(b-a)
            return r;
        }
    }
    else if (_Base.flag && _Subtraction.flag) {//negative - negative
        //negative - negative = negative + positive = -(positive - positive);
        Number max__ = Max(_Base,_Subtraction);
        if (max__.Value == _Base.Value) {
            Number r = _Subtraction_No_Flag(_Base,_Subtraction);
            r.flag = true;
        } 
        else {
            return _Subtraction_No_Flag(_Subtraction,_Base);
            //if _Subtraction > ; number will be positive anyway
        }
    }
    else if (_Base.flag) { //negative - positive
        // <==> -(positive + positve)
        //that one's EZ
        if (_Base.length > _Subtraction.length) {
            Number r = _Addition_No_Flag(_Base,_Subtraction);
            r.flag = true;
            return r;
        }
        else {
            Number r = _Addition_No_Flag(_Base,_Subtraction);
            r.flag = true;
            return r;
        }
    }
    else { //positive - negative
        // <==> positive + positive
        //EZ as well
        if (_Base.length > _Subtraction.length) {
            return _Addition_No_Flag(_Base, _Subtraction);
        }
        else {
            return _Addition_No_Flag(_Subtraction, _Base);
        }

    }

}


Number addition_Numbers(Number _Arg1, Number _Arg2) {
    //This is the called function.
    //It will then handle flags and sizes with other child functions
    if (_Arg1.flag && _Arg2.flag) { //both negative
        if (_Arg1.length > _Arg2.length) {
            Number r = _Addition_No_Flag(_Arg1,_Arg2);
            r.flag = true; //set to negative (negative + negative = -(positive + positive))
            return r;
        }
        else {
            Number r = _Addition_No_Flag(_Arg2,_Arg1);
            r.flag = true; //set to negative (negative + negative = -(positive + positive))
            return r;
        }
    }
    else if (!(_Arg1.flag || _Arg2.flag)) { //both positive (basic addition)
        if (_Arg1.length > _Arg2.length) {
            Number r = _Addition_No_Flag(_Arg1,_Arg2);
            return r;
        }
        else {
            Number r = _Addition_No_Flag(_Arg2,_Arg1);
            return r;
        }
    }
    else if (_Arg1.flag) { //only cases left : only _Arg1 is negative and only _Arg2 is negative
        //negative + positive = -(positive - positive)
        //CAREFULL : the returned value can be negative, so the flag has to be a xor with current flag
        _Arg1.flag = false; //temporary set the number to positive
        Number r = subtraction_Numbers(_Arg1,_Arg2);
        r.flag = r.flag ^ 1;
        _Arg1.flag = true;
        return r;
    }
    else {
        //positive + negative = positive - positive
        //though it's litterally just a subtraction
        _Arg2.flag = false;
        Number r = subtraction_Numbers(_Arg1,_Arg2);
        _Arg2.flag = true;
        return r;
    }
}

/* ----------------------------------------------- */

Number _addition_uint(Number _Arg1, Number _Arg2) { 
    //Doesn't optimize memory ; takes 2 same-length Number so I don't have to check the size difference as well
    //Add _Arg2 into _Arg1
    //Has to be used INTO Multiplication_Numbers because it DOESN'T handle different sizes
    Number result = Create_Number(_Arg1.length); 
    //Takes for size the same as the rest
    //Why ? because _Arg1 and _Arg2 have for length the MAXIMUM of the multiplication of the 2 Numbers (in Multiplication_Numbers)
    uint16_t _number = 0; //intermidiate
    for (uint64_t i = _Arg1.length - 1; i < _Arg1.length; i--) {
        _number += _Arg1.Value[i] + _Arg2.Value[i];
        result.Value[i] = _number & 255; //takes 8 first bits
        _number = _number >> 8; //keep the retenue
    }
    return result;
}

Number bit_shift(Number _From, uint64_t number) {

    uint64_t nb_octet = number / 8;
    uint8_t nb_bit = number % 8;
    
    uint64_t length_ = number / 8 + _From.length;
    if (nb_bit != 0) length_++;
    Number returned = Create_Number(length_); //handles problems
    uint16_t temp[2] = { 0, 0 };
    if (nb_bit != 0) {
        for (uint64_t i = _From.length; i < _From.length+1; i--) {
            temp[i % 2] = _From.Value[i-1] << nb_bit;
            returned.Value[i] = temp[i % 2] & 255;
            returned.Value[i] = returned.Value[i]|temp[(i + 1) % 2];
            temp[i % 2] = temp[i % 2] >> 8;
        }
        returned.Value[0] = temp[1]; //the loop will alwyas end at i = 1 so we just do as if i == 0
    }
    else {
        memcpy(returned.Value, _From.Value, _From.length); //if it's moving just bytes, then it's easy
    }
    return returned;
}

uint64_t Get_Number_of1s(Number _Arg) {
    //so it's a basic for loop that check for each byte, each bit
    //complexity : O(8n) with n the length of _Arg
    //there'll be problems if the length is 2^61+ (then 8*length is an overflow)
    uint64_t r = 0;
    uint8_t _temp;
    for (uint64_t i = 0; i < _Arg.length; i++) {
        _temp = _Arg.Value[i];
        for (uint64_t j = 0; j <= 7; j++) {
            if (_temp&1 == 1) r++; //checks if 1 or 0
            _temp = _temp >> 1; 
            //got told >> n is n times >>1 so i guess it's better than doing n(n+1)/2 shifting
        }
    }
    return r;
}

// -- Now, let's do a function so multiplication_Numbers only use Number of ones and this function to get the result
//(it makes it easy to implement karatsuba algorithm because i can then check if the number is long enough to cut it in half))
Number naive_muliplication_system(Number _Multiplied, Number _Multiplier) {
    //basically, multiplication will do the optimal way (take number_of_one, etc.) while this function just multiplies
    //_Multiplied will be the one to be bit shifted according to _Multiplier's 1s positions
    Number Result = Create_Number(_Multiplied.length + _Multiplier.length);//the Result ; maximum length is the addition of both
    //no need to verify, it already handles that in Create_Number
    Number Inter_Operation = Create_Number(Result.length); //intermidiate ; same length as Result
    uint8_t temp_;
    for (uint64_t i = _Multiplier.length - 1; i < _Multiplier.length; i--) {
        temp_ = _Multiplier.Value[i];
        for (uint64_t j = 0; j <= 7; j++) {
            if (temp_ & 1 == 1) { //check if it's a 1
                Number temporary = bit_shift(_Multiplied, (_Multiplier.length - i - 1) * 8 + j); //bit shift
                memcpy(Inter_Operation.Value + Inter_Operation.length - temporary.length, temporary.Value, temporary.length);
                //No need to set Inter_Operation back to 0 : temporary will always take up more space so it'll reset the memory it had anyway
                FreeNumber(temporary); //frees it (less memory usage)
                Result = _addition_uint(Result, Inter_Operation); //add to Result

            }
            temp_ = temp_ >> 1; //shifts
        }
    }
    Result.flag = _Multiplied.flag ^ _Multiplier.flag; //does a xor cause - * - = + and + * + = + while if only one - it's -
    return Result;
}

Number* Split(Number _From, uint64_t where) {
    //splits the Input to 2 numbers, cutted down at 'where'
    //r, the return value, will always have a length of 2. r[0] is the low part of _From and r[1] is the high part (_From = r[1] << where + r[0])
    if (where > _From.length) {
        Number* r = (Number*)calloc(2, sizeof(Number));
        r[0] = _From; //if where too much, the low Number will be the input and the high will be 0
        r[1] = Create_Number(0);
        return r;
    }
    else {
        Number* r = (Number*)calloc(2, sizeof(Number));
        r[0] = Create_Number(where);
        memcpy(r[0].Value, _From.Value + _From.length - where, where);
        r[1] = Create_Number(_From.length - where);
        memcpy(r[1].Value, _From.Value, r[1].length);
        return r;
    }
}

Number karatsuba_implementation(Number a1, Number a2) {
    //this version frees the args. DO NOT USE OUTSIDE FUNCTION
    //Child function, DO NOT CALL
    if (a1.length < 100 && a2.length < 100) {
        uint64_t Arg1_number_of_one = Get_Number_of1s(a1);
        uint64_t Arg2_number_of_one = Get_Number_of1s(a2);
        if (Arg2_number_of_one > Arg1_number_of_one) { //takes for base the number with the least amount of 1s (so less additions & bit shifting)
            Number r = naive_muliplication_system(a2, a1);
            FreeNumber(a2);
            FreeNumber(a1);
            return r;
            return r;
        }
        else {
            Number r = naive_muliplication_system(a1, a2);
            FreeNumber(a2);
            FreeNumber(a1);
            return r;
        }
    }
    uint64_t temp__ = max(a1.length, a2.length);
    temp__ = temp__ / 2;
    Number* split1 = Split(a1, temp__);
    Number* split2 = Split(a2, temp__);
    //free the numbers
    FreeNumber(a1);
    FreeNumber(a2);
    Number add1 = addition_Numbers(split1[0], split1[1]);
    Number add2 = addition_Numbers(split2[0], split2[1]);
    Number s1 = karatsuba_implementation(split1[0], split2[0]); //after that, split1[0] and split2[0] are freed
    Number s2 = karatsuba_implementation(add1, add2);
    Number s3;
    if (split1[1].length == 0 || split2[1].length == 0) {
        return addition_Numbers(bit_shift(subtraction_Numbers(s2,s1),temp__*8),s1);
        // <==> return (s2-s1)*256^temp__ + s1
    }
    else {
        s3 = karatsuba_implementation(split1[1], split2[1]); //after that, split1[1] and split2[1] are freed -> no usage of freed number
        return addition_Numbers(bit_shift(s3,temp__*16),addition_Numbers(bit_shift(subtraction_Numbers(subtraction_Numbers(s2,s1),s3),temp__*8),s1));
        // <==> return s3*256^(temp__*2) + (s2 - s1 - s3)*256^temp__ + s1
    }
    return Create_Number(0); //returns an empty number (debug ; makes sure it handles the error for now)
}

Number multiplication_Numbers(Number Arg1, Number Arg2) { 
    //Called function
    //handles small number with the naive method, else does karatsuba.
    //SAFE : doesn't delete the Input
    
    
    //Karatsuba implementation :

    if (Arg1.length < 100 && Arg2.length < 100) { //The 100 here is totally arbritrary. It got chosen because it was the fastest between 10, 100 and 1000

        uint64_t Arg1_number_of_one = Get_Number_of1s(Arg1);
        uint64_t Arg2_number_of_one = Get_Number_of1s(Arg2);
        if (Arg2_number_of_one > Arg1_number_of_one) { //takes for base the number with the least amount of 1s (so less additions & bit shifting)
            return naive_muliplication_system(Arg2, Arg1);
        }
        else {
            return naive_muliplication_system(Arg1, Arg2);
        }
    }
    else {
        uint64_t temp__ = max(Arg1.length, Arg2.length);
        temp__ = temp__ / 2;
        Number* split1 = Split(Arg1, temp__);
        Number* split2 = Split(Arg2, temp__);
        Number add1 = addition_Numbers(split1[0], split1[1]);
        Number add2 = addition_Numbers(split2[0], split2[1]);
        Number s1 = karatsuba_implementation(split1[0], split2[0]); 
        Number s2 = karatsuba_implementation(add1, add2);
        Number s3;
        if (split1[1].length == 0 || split2[1].length == 0) {
            return addition_Numbers(bit_shift(subtraction_Numbers(s2, s1), temp__ * 8), s1);
            // <==> return (s2-s1)*256^temp__ + s1
        }
        else {
            s3 = karatsuba_implementation(split1[1], split2[1]);
            return addition_Numbers(bit_shift(s3, temp__ * 16), addition_Numbers(bit_shift(subtraction_Numbers(subtraction_Numbers(s2, s1), s3), temp__ * 8), s1));
            // <==> return s3*256^(temp__*2) + (s2 - s1 - s3)*256^temp__ + s1
        }
    }
}




/* ----------------------------------------------- */

char* convert_to_str(Number from_) {
    //BASE 16 REPRESENTATION
    // --- UNUSED ---
    char* returned = (char*)(calloc(from_.length*2+2,sizeof(char)));
    if (returned == NULL) {
        exit(47);
    }
    returned[0] = '0'; //le "0x" au début n'est pas obligatoire, mais je l'aime bien là où il est
    returned[1] = 'x';
    int a = from_.length-1;
    char constants[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'}; //ça évite d'avoir 16 if/else
    for (uint64_t i = 0; i <= a; i++) { //on commence du début car little endian
        uint8_t visualisation = (uint8_t) from_.Value[i];
        returned[i*2+2] = constants[visualisation/16]; //prends les 4 bits du haut (1010 0101 -> 1010)
        returned[i*2+3] = constants[visualisation & 15]; //prends les 4 bits du bas (1010 0101 -> 0101) (grâce au & et parce que 15 c'est 0000 1111)
    }
    returned[from_.length * 2 + 2] = '\0';
    
    return returned;
}

/* ----------------------------------------------- */
char* number_to_base10_str(Number from_) {
    Number copy = Create_Number(from_.length); //copy from_ to not destroy the input
    memcpy(copy.Value, from_.Value, from_.length);
    uint64_t number_of_zeros = 0;
    for (uint64_t i = 0; copy.Value[i] == 0; i++) {
        number_of_zeros++; //permet de ne pas avoir à allouer trop de mémoire 
        //(normalement, c'est pas trop long puisque mes fonctions optimise les Numbers)
    }
    uint64_t char_len =from_.length-number_of_zeros;
    //pareil, on a : 256^x = 10^y ; xlog(256) = ylog(10) <==> y = xlog(256) (log(256) ~= 2.41)
    char_len = (char_len * 241)/100 + 1;
    char* char_returned = (char*)calloc(char_len, sizeof(char));
    if (char_returned == NULL) {
        PanickError("char_returned calloc failed (number_to_base10_str)");
        exit(0x10001);
    }
    memset(char_returned, '0', char_len);
    uint16_t truc = 0;
    uint8_t reste = 0;
    uint64_t pointer = 0;
    char contant[10] = { '0','1','2','3','4','5','6','7','8','9'};
    
    
    while (number_of_zeros != copy.length) {
        for (uint64_t i = number_of_zeros; i < copy.length; i++) {
            truc += copy.Value[i];
            copy.Value[i] = truc / 10;
            if (i == number_of_zeros && copy.Value[i] == 0) {
                number_of_zeros++;
            }
            truc = truc%10;
            truc = truc << 8;
        }
        char_returned[char_len - pointer - 1] = contant[truc >> 8]; //quand on arrive à la fin, on a le reste qui est entre 0 et 10
        pointer++;
        truc = 0;
    }
    free(copy.Value);
    //on va compter le nombre de '0' inutile dans char_returned (ceux à gauche)
    number_of_zeros = 0;
    for (uint64_t i = 0; char_returned[i] == '0'; i++) {
        number_of_zeros++;
    }
    memcpy(char_returned, char_returned + number_of_zeros, char_len - number_of_zeros);
    realloc(char_returned, char_len - number_of_zeros);
    char_returned[char_len - number_of_zeros] = '\0';
    return char_returned;
}

/* ----------------------------------------------- */
int main()
{
    if (CConsole) CreateConsoleWindow();
    char a[16001] = "1999999999973237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173212173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173212173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173212173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173237892793289112173214377954443388267280640888218163624769328114904247290516526028112562006956224241244368421808377222615705918995560323522091585247153520522872302360390671482710904475133473295543448303205355476795969301371123640339316490939019966713931322254677199489271073456232005628162826122967870638432726046823763007365823553584993432595315984029275287902810665959505387174591332989337337003495570101126806963733839735929420789657747133854751230953966664015630180955440579041285890051662606954379242478701167142942849204828954643887655862518018800671110928287341494785631254509411579275871169367665874298060800995520696138291532924283639780999153003449833974058029825013194961091582935920103498734217651702837948331674654566163847884134274660055598872302251540997179414943005878794378340813251958711238170077500805027320605968487801530332383612690310869315539490280391966316222029468295159349257402942127843606801955390910800275141313117810337801600880104561921177071313881336812946515202462167600564927188851881223142172640852593029584227812296822122654326707547141897303928931320885909777114063293498676465194706571964383720562611177907185925577358734181995856861873950232147961009811370052726687863966752509207608558649079879120440809451144263159479161318438157056804304940659749454475833334623969362816329056442898666382856718620872343041852675001686208979807611180896767939162641621727492599384123885117607616591423407166416576636998595470709642459762822974731480649488232988803477704635046603077059095742709406424102885746628692083748371901120245582641905465798176109214584662366756835853596454258796632710860802509903676136058170551533353674888330017851297995901041475385141075209549480544647620147955934555917743835368120179195665884146622887987512787695853930985250554765940056030394493711167747173444160169537127037397309079604087277396241467839490249121713676511490374335512556517171220552924365241361518976017890232489456246861129448032089098760427179202316207150529907255020242246549086601900971792302136970898657559043054663050964544967414393824232497158794345898945324843264485591296501515489283158556405705878556868768665091675848624796342679090392234240773183642492737041731073502549956190204236549248374899934068143338581956611140136201940087331914713486708077861022774421420459596538707773695412520205659137599455204941011023735346008813981135964262814529574080816358250551889765505577546684511118880499324577508982928507666557936391784580151500118012658400220597928086008592069263693900627223437167690856909679505518919063116150607783360560194057880811631831220433602278480006017827565583393302112622440558915287069177299737111616296883991573115418266510894469241101641842671968735977598046716911693049503863804261131506277123840492864563143757337317046371360709693733548638578784465676056603447029047617765806216711147698975651131631442356874124520111149985234666875097278920727661540729068292760889013663723052282958143826642652260948644586656560181856732890722874587838599151820444206565051388688361609856497210400410566755484000666161758297930438118985321159285063255017601375316134826753320325951089815568835792443909357899650413602190578654488406015709103603161716725509752929940538596786331916742735991097204721257165678598205919818612045331422917209573235484878459735467376540089768870970812527226278934229994618943619628158742009880100356211944767856362178959535845079558361877016918675291857150126026962478976131575000797070300751907466865148078487709678612314830300986148711093442552582070828432185313983572696747228240960475363779395060350098664224292637147928925635555007883796185189237565991825940727234747494223262621267116246301276182857459156202210381837101967277549407859303117128777498493681317664149192210220339607953788085568684143055216668189644294266308653015481750970171359698166155422321353242140319523637558707834433184233784954811734295459713790945111369778624259656385593285674810912820143903614034845293815625662708249056647899224054621034136745179500614643036738113578267702351949806471037036035953814316387300171047378923667683239356359113040025608002614559076507614542765324315779784761882370569446690828613543590718536018667014380549779746398476234027876104940024100066101976459609144019526841161758452179645201485331559335283141096298803166529325069015686007431345026610324012232054306969514779781529641800440803732253671702911859905852403967093132378412665888920648952807446467921792995925564498251115736816611797316943675344176267093395451499447202240442988077221668385045215072754146387876198986703999996060427319391852077583869118203689665910932685582186168416312011203797129989105183407349496907438968414829635560315354513340238683292952255687680071344987953698739407963177552351985429504508625822455337670769456167940682155402258934030481830098875552299883756750812291541901415848750729443043154134261158903705255371235274254972692859747722353128505346141749617305136243583991190841167536170090609606806201873806366296299962018946078049753552637587995753";//input, put any number you want (base 10)
    char b[16001] = "5300766670760009640118988458725690157796967659726694165876283699204681064127491437247081031193088431092318768695887624090115845241576832165025171096920701418358467589086773491457372944954420658141388944299417811816085360389448304936266888055982633901636703857960755983472185081124460729146377555467468646971252432418059015822318464548025546012551352340596756211806577058564961731586444854456271622762413317012194615856369356084305505577834468319730459379035965801500120354380339662759543655930775808852320815185557110665652903203958055913500580509026980853917972809017034982054266507761121307875714092464657563038146685954598630084761512199123184442252593597734122490519680448750624783662754748899422350672119069763124399160137177521590314669551828990608525277475179409632071433465150932374435885278385298581248120222711278974822219260043735526587607096395562646594137859285351850048894439566623335868689295097230923992296007595017242980834843897621383935848718934505302968358299376436585127597639285982086461752587717112238065100952994711624353623691791918842940512958661926099634659556745562199144663078228569152612554707549148128881351182970637206776591849720360767595942238174526926764035944421015535131585109048086426701170380527389060703378990565674616961339720993558561173283143074213388741897501398319325713192926457471134925264020456280284592826711032510575035973923500270805239310741255622286187037012096248979893399959382492910141291558652812667866549358419262633521917054655067356482988836197803529702622896509223860139381353872834601882825528452555385804667929107154175847366127111348696263052121026033624743206514997251644931876070603375763908275170104258455052638030594029388527790438954102221688777528710383195031954937650495738587767186497396781880911524353268317578391886888399789533635422129453396063860036480692225370039914200998149236213946847385404418087615158793189201774161735444319301964121183039426547014978424268263411901101376575648202968217048371768181078965446524424213688135538070019760214281985444893557968453233633219821357869084454236764703393212871266567776727173476606132912601363576077150071369573720069808567508607178509012035420492379310061742799771286997478808220608724105892154988740843232198885198915767125216372845425847798796627017599063844517924186803557546238636048298553130203586881593145720794076151694120274253406331220803880754609094120786971396288506825002200388580657447130162854663100909375297069138121402212832153555546836920396774081005240161334126172244904639991465771714260253169829727097990029166795784947978888074251129363462076493759766358857601022744124534200589503891319205335412859969605417243115136325504802252043976645444968727079092365015803855181418679942705473634758035782028282674827949134379104684993987863158459195337750130248364164094144192481735988505922262849723239989378796348856385286899642153158550920796115446270972275416294069136039621668801838589768685294631722432100045308378206222884632001587859271720334264810809909782229293614837140201344082402386325485395042137097564823824254521882788097368654415598605260668781561608879922386352547385850896047008814370681685604016419391434500127320042537167197707356504220400190362795338778372844264655883829466466507148667137014764487047486758373168655057002907606836067042904457159057644733816527639784369337782167785047773603084122599244204550331038292208600912767512882574134176730759832601172835502511414941622631595704781813710746139212343459467481571332880184047458991122000409573057575292475078506566325931009722899443081498302266333856532792390221326932118318499126451848690085593610222499987481262375818218451605287945847730058855793623538675117639496067659428634029022912796681723953473641388097287705529491212186190815346420174623109741349076992624305676523336626362529269007696106198435192403371748307192776087933593051219069575772362150098328846763166539129873494720375439038668683002293899919312779577422316095868988708607924991373622626046379300335663386187512857804672295293659356973016533318673204076205709692916147509841521476896924579435405688502966677504942501012998918012027173240928991917797670530270915504563177776857100534892945533101439290735205030691411782157012593656661643305735058265169192464636923366909096108878707031421653390201962228692685467009435074855823763072158464351373068126011621649376430956054035303710689434525776563344544918395375601651877021569664893011138878978528330921636962665771546595620433629235348171734966651319956398571372544646102788729669485903960051859407667215085141782091343814989245490999968686331302067308153007451477664401876948917924084441986480572763966388385430158833512828258565865018589781998130816550865421654309335252634341389808839129929691318443848503677457085042809200153397924862867080391259914805236498305068568184305837057952630214104293671558539281672044981333203370376265221446947144990356892077080459802422917457058202604145184705577620121376592980103132062654660103710626393903901445640505160838463674289415465537742206658122317280631630255486600433140795751274294693973229541088951342631626096673579314623076378078142205459776011916446036765186731059207177412754250345608521415209309122945737296504135656297963111052041236069432710829333308025340170547994000960145024577867302137927246721911216175786102786606156148918198096827187891616955041524135378577566122182503973043485874443635141725283946609884126965577269587498461990217160526637846432161920924134070897820145420636469560604159550140010396936926126492376173479844224894358561169144075754876568790626876468328521691858098723330801263279935590935527487898065056827710923064214150641270250355035866247784358420597746031774370852910018872176307162261290925611157309331779297074724958567719143742666310143606476674400017042291692422298057439266536065187989972522789925070594749549019853786942643611497828944360671364789281510204506208400684784565275031181639323104361850687048397463186357346643379052194755636941503575696914494295980303057184017347361623671277883176239482804907378060076685136190425357838285071606998188087715034772505422862030771577813106049717151674961019053154200458588835853329449912674457664568276226475527220422624870282269168173010865167927376388792505511167181685359132391924396060647138729031064746460782600379031222266061113649660267433534035948345578815677841315998580649404593329497265563608021436854296858134127096560567687858502043485047508585065974429344165821767922252724501626686805556216754793437170613674206095185014021994328090372766641142636989917157503379586538685082300427534160467881595382061032890265457813889172000426576350200199421051552318975064364155042510145428131465653558270794586075020634107514824405020634768326105494275181759841817800832072045042694560102503194088075615919019823450662503088813736067210217086265405451511242007022460500488517866639559334492571306898519810573475494567515047052995678488926974503191436118954515268512224889588513221160603532662579524461763300631798701140581076642732718396193300642330653573503919665980050438456462072662908286955707275311593120323937923219141031158211860340538523040648839747709139594420633896648865647575785668042749757778120777331957800014284298120700730975334179834473792725975736474465935152635011138744675623386570966753029158711004031411234694365395061474001742226382901093411304383347823521978588927121051258132419660718942219480265789743669165928578169795812534283091481007528721568092684910459178409616957523610574960302044105191000784685701534975685672656118759049399140366567331297067133515000980986126042970706228084596423408189297616461698076426483511809486737613181997039275022739977898684255712390455886944517788218012278309853790803797946750346092097314473990747089438358303916733174399905955561414771373296635600228959705820723935431854905100069839528753811178301594420208276038681653368887385270781081479676497249264400524420865919353665069516835205628486489884476461449449000774255483972716973463741937839708157199681729699917835872734301990520020294212522183410000200280785001261550993911008310063126671436692762308860011301140484456857583293168512188025166749750360105551235901265185002465179555986744833627843864373981024189060233107216286856132944068786074176367656052277484513757640972458356379955228225363635459229727370925394666610040988046051242632956951914649173711548886413245058345191619365665107844930837752259586239688975659881380987108625052220710756945295869647804295093379163003812289536874477813383875101638549943809141493046461650697783396706716299262962233039887961974807577788910743892484223936697022179832118871016949204395321776968605270778343423038784968055559101607447849737662820912165564894029075896592365898080667222546007567962971983871366158307990117374773660318703721553263069605118626671492470923260998211916275847955880042010734751322619468415477141119442859595664878062662850771867722955014377169641425142074886047619266380871032501193835222157341798927209227167591306180112636049801266297423806347817033557526076699468187221217314807383927691464720451617562038610230687166282086684888860095666296692675977933095686880308531441462644816504775979386242097081535606749809511090006568993740027005263327635936797965637988962555792625127327816791937851788032619538354293724553902540060322147837611108136674327947912787171891122630829029051823153557367297245265277048438835382788612416598991358478427004529400531432968799822619024781400292129829783194558216205707697179261847735967062036201636411028076579602968658469027125413166088088668086952006682764446412043587353571341592050116036858408881231990441367230068456297649732959481900392439565427805402935417646225317006123157552711021438115757407035277444438319902437309475051855357967455852201297701930457387537839044609191063666206417757124140544884472176201713969401709585418796878714690957396399826830040894666721965343308300621347946866054782348333691071378650666884658685795623786036863885556541376612854598980236212999109087395412575958693714596241604950949717979888795166749389894949086400927791906478772455195608428612151945560904238913406207445383830551737112861619070871098279796602874887557254610188926873873786889473825331118251074077274904333223363909431916766534647324281332012195276305666569972561156886812365121797161441928919271402685914009651885144163926176864645649452678737780293141167284512190485039129334596870428844187476716520957365118949959569273261950956975809301415037737252450453713749342933233709351077773771155890685164234831053450989645234955530131172661180059650850303750986886655009857264514809153712609917376930161460647766745342713603164161570320141766784587901431248581779770118692335187152980334211668931984282106132993200178053520559951489755748285243830872308248878006526232056894559775379038601208008805876159616585085567145063432210415189944296005223066271520629381130757614986384225226615197714653477198734625605685524665827860748551960493311520801244992976464019701827444457914932871714500704554786505613266603850167830403254097109991009922163517484591214934040594848525749612598088813202912161250573939659350029532462609919068063494282272006987664747214834723143153188061919663879874660734589238240614766054286939744121984357768412475183666813695567502502604809150371379675973113729077218623523062017329282225044855813354159769862939543037488145576393943130216969050497856692744671003447377035090959973361223248687688603228103067751337860140353845653623101014218385945707516149304831230470815240944574520954527349594097334021508192728039860995586372031374456599399264748705680500883079595246384683452161872329187717038616939944815557500245830696587754305334617326259208257476590122616267137196482203278451914458222464394875905922911401186163256845823046238013535084795751413350220309299305080375702665593737326239190697100603856404377653383229229387990584028229536846720850422132699240702898870422007621970955794108558309547999402506022240423082209980990645041617719851872334753759663754179570551752731159505440604615115359895044145955288917285545108190652656233109867104899460363102672047037553942654979058781841080775538702384834075764529556922678644952691067528812105176397021607400327358689807364782505501057045743904000980876844205577479583882064877272545747115378767591226143078243072348226721312696050428632899285075878457820972376789446909874295317441981730520471290822178015008150626946383351369321168588505794997492501149769947775086378706436501960113420866463575615496747939563998647094846432219146863410830832065576217366970923794604367055913985906609409791599768645224300572968630451267811505806471776179578535180906265053832477479959640652066151819185906751455977082328838600558166581671817498570446741073920721105674867168424179123343272385668804215791208393703221674340430208634291983003233249287217938365614919348104885672633105216917131254447617321433572797000838660449020217562921220889706794242040372950405845108523078303681681859525087624179052636369408228138033449141539991680729074334050356085241867865252044758429037004748830024013959795899279979474004946764357288124859418970274911917557268901777725802226032712973802831654218784922034541356960735705824798787090632599621192514420685742762336871219852308163472166566995841191279245091082016380404098731090267286240382206187594024379722073013962620537742324242697894955740293793010828303263636742280035020420354900312550982198584258259813804598357265417075487087934007364375099424498593805759668209269991461815150552243861355380099492393434463162797212956444528108896287255914245494864412583476542307830088685659314654567844138138482553570947034744732900358632293843694434125627330287231607235016411608328679388312250714840071389922844073946101011675807986701901291256604327712155469622299835159829283186132369274692975419787299906594039422347313559525262313076764266489540806726418689659403011210370348787635486198811398216546911656323143781271375284628235315692527242526967892429938937100486734763979952581812515257329002028894759402760833819716789895495668100429939371431468529769423921807848116626650802799556916268130853420457690790182064526494126119381868846263918666974786302207416942609820286819899143546013842555619465370267052102078924877917660086908131684635370021485833867814578090806276281573031509985833655921287852018339764951133643600404437358314559836638622961712618042244120183970064964149652583011275479711288582467114520585320033834139352629197423033620250575271037183057354277133954124933172305414977341761197952922119483999255565778588499233777212583908200543929004632169575386122157639217811954756236106770691030233403277112548920636816294254678167266501722921489663888586379463716663307294463894659634745483010439389078504696091002378685220231130281018312274548485561889524464399472950229833345416252386374410062079025547112945611038593254760851217669804529661526014740672835120973560900341758275480346924487383507952055678672917598198418018516756247478685925985286258326798031409957447114591851456714085337670769456167940682155402258934030481830098875552299883756750812291541901415848750729443043154134261158903705255371235274254972692859747722353128505346141749617305136243583991190841167536170090609606806201873806366296299962018946078049753552637587995753";

    timespec my_time = { 0, 0 };
    timespec new_time = {0, 0};
    if (timespec_get(&my_time, TIME_UTC) == NULL) {
        exit(61);
    }
    Number a_conv = parse_base10_str(a);
    Number b_conv = parse_base10_str(b);
    char* a_str = number_to_base10_str(a_conv);
    char* b_str = number_to_base10_str(b_conv);
    Number c_conv = addition_Numbers(a_conv, b_conv);
    char* c_str = number_to_base10_str(c_conv);
    Number d_conv = multiplication_Numbers(a_conv, b_conv);
    char* d_str = number_to_base10_str(d_conv);

    if (timespec_get(&new_time, TIME_UTC) == NULL) {
        exit(60);
    }

    //calcul du temps que ça à pris (simple soustraction)
    long final_time = new_time.tv_sec-my_time.tv_sec ;
    long final_time_2 = new_time.tv_nsec-my_time.tv_nsec;
    printf("nombre a base 10 : %s\n", a_str);
    printf("\n");
    printf("nombre b base 10 : %s\n", b_str);
    printf("\n");
    printf("addition a+b : %s\n",c_str);
    printf("\n");
    printf("test of multiplication : %s\n", d_str);
    printf("\n");
    if (final_time_2 < 0) {
        final_time -= 1;
        final_time_2 += 1000000000 ;
    }
    printf("final_time : %ld.%09ld",final_time,final_time_2);
    printf("\nin microseconds : %ld micro seconds", final_time * 1000000 + final_time_2 / 1000);
    return 0;
}

