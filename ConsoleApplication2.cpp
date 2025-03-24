//projet calculatrice qui prend toute la RAM

#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <wingdi.h>
#include <time.h>


uint64_t uclen(unsigned char* input_) {
    uint64_t returned = 0;
    while (true) {
        if (input_[returned] == '\0') {
            return returned;
        }
        returned += 1;
    }
}

//Partie 1 : faire la multiplication simple
//Il faudra lire les bits directement (sinon la v rification du nombre de 1 sera compliqu e car elle devra se faire   la m thode "classique")
uint8_t* conv(char* _Input, int* result_lenght) {
    unsigned char* final_variable = (unsigned char*)_Input; //convertion de _Input (variable d'entrée) en un truc plus utilisable
    int number_of_zeros = 0; //ça ça va compter le nombres de 0 depuis la gauche de _Input (ex : "001234", là ça sera 2)
    
    // verification de final_variable (si il y a autre chose que des chiffres)

    int a = uclen(final_variable); //évite de devoir faire des appels succesifs de uclen

    bool count_z = true;
    for (int i = 0; i < a; i++) { 
        if ((48 > final_variable[i] || final_variable[i] > 57)) {
            return (uint8_t*)calloc(1,1); //ce n'est pas un chiffre on retourne un uint8_t* vide
        }
        if (count_z && final_variable[i] == 48) {
            number_of_zeros += 1; // ici on va pré-compter le nombre de 0 depuis la gauche (car si quelqu'un mets "001234", ça va pas marcher)
            //on ajoute donc à number_of_zeros tant qu'il n'y a pas eu autre chose que des 0 (tracké par count_z)
        }
        if (count_z && final_variable[i] != 48) count_z = false;
    }
    
    uint64_t length = strlen(_Input)-number_of_zeros; //longeur de returned_int (calculée en avance) (on fait attention au number_of_zero) pour ne pas utiliser trop d'espace
    uint8_t rest = length % 3; //Ici, j'ai utilisé le fait que 1000 prenne 10 bits et donc je fait un calloc plus grand pour ne pas avoir à faire de realloc après (le calcul pourrait être optimiser je sais)
    if (rest != 0) {
        length = 1 + length / 3;
    }
    else {
        length = length / 3;
    }
    length = length * 10;
    rest = length % 8;
    if (rest != 0) {
        length = length / 8 + 1;
    }
    else length = length / 8;

    
    uint8_t* returned_int = (uint8_t*)calloc(length, sizeof(uint8_t)); //on fera une liste chaînée d'octets (0-255) (LITTLE ENDIAN)
    if (returned_int == NULL) {
        exit(45); //code d'éxit différends (pour savoir quelle alloc a foiré)
    }
    uint8_t current_number = 0; //stocke la prochaine valeur à ajouter à returned_int

    uint64_t pointer = 1; //compte la place que current_number devra modifier

    int nombre_operation = 0; //tracke la position du bit qu'on va ajouter à current_number 

    while (final_variable[0] != 10) { //la boucle finis tout le temps (on set final_variable[0] à 10 si nombre_de_zeros = a et il ne peut pas avoir deux zeros qui spawn en même temps de toute façon)
        if (final_variable[a - 1] % 2 == 1) {
            final_variable[a - 1] -= 1; //si le premier chiffre est impaire, on le soustrait (pour en avoir un pair) et il nous servira a faire current_number
            current_number += (1 << nombre_operation); //on utilise << pour bien décaler le bit
        }
        nombre_operation += 1;
        if (nombre_operation == 8) { //si c'est 8, on a fini l'octet, on passe donc au suivant
            returned_int[length-pointer] = current_number;
            current_number = 0;
            nombre_operation = 0;
            pointer++;
        }
        for (int i = a-1; i >= number_of_zeros; i--) { //on peut du coup aller jusqu'a number_of_zero exclus qui donc enlève des opérations
            if (final_variable[i] % 2 == 1) { //si chiffre impaire
                final_variable[i] -= 1; //on enlève 1
                final_variable[i + 1] += 5; //et on rajoute 5 au chiffre après (ex : 52/2 -> (i = 1) 2 est pair, donc on divise par 2 (-> 51) ; (i = 0) 5 est impaire (et 50/2 = 25) on enlève donc 1 (ça fait 4) puis on divise par 2 (ça fait 2) puis on ajoute 5 au chiffre d'avant (-> 26)
            }
            final_variable[i] = final_variable[i] / 2 + 24; // on divise après par 2 en sachant que le chiffre visé est pair et on ajoute 24 (puisque le code du chiffre est 48+chiffre il faut donc corriger le 48 qui a été divisé aussi)
            if (i == number_of_zeros && final_variable[i] == 48) { //si le chiffre le plus à gauche qui n'était pas un zero devient un zero, on ajoute 1 à number_of_zeros
                number_of_zeros += 1;
                if (number_of_zeros == a) { //si il n'y a plus que des 0, on change final_variable[0] à 10 qui arrêteras donc final_variable (il faut juste pas mettre des nombres entre 48 et 57 car ce sont des codes de chiffres)
                    final_variable[0] = 10;
                }
            } 
        }
    }
    returned_int[length-pointer] = current_number; //quand on sort de la boucle, il faut quand même changer le dernier returned_int
    

    //TEST : Faire un realloc depuis un pointer modifié pour enlever les 0 inutiles
    int64_t test = 0;
    for (int i = 0; returned_int[i] == 0; i++) {
        test++;
    }
    if (test != 0) {
        length = length - test;
        for (int i = 0; i < length; i++) {
            returned_int[i] = returned_int[i + test];
        }
        returned_int = (uint8_t*)realloc(returned_int, length); //il y aura assez de place, puisque la taille qu'il prend est plus petite
    }

    (*result_lenght) = length; //on change la longeur (donnée en entrée) car il faut la tracker (sizeof marche pas et si on cherche un 0 il peut y en a plein)
    return returned_int;
}

/* ----------------------------------------------- */

//fonction addition : c'est simple, il faut juste ajouter chaque uint8 dans un uint16 , prendre les 8 premiers bits et les mettres dans le uint8 du résultat puis mettre la retenue dans le premier bit (pour le prochain calcul) ; règle première : ne pas changer les variables d'entrée
uint8_t* addition_uint(uint8_t* _Arg1, int _Size1, uint8_t* _Arg2, int _Size2, int* _RSize) {
    //_Size1 -> len(_Arg1) ; _Size2 -> len(_Arg2) des _Size inexactes peuvent créer des erreurs
    //_RSize aura pour valeur à la fin la taille de resultat
    bool arg1_bigger = (_Size1 >= _Size2);
    int _min_ = min(_Size1, _Size2);
    int _max_ = max(_Size1, _Size2);
    int length = _max_ + 1;
    uint8_t* resultat = (uint8_t*)calloc(length, sizeof(uint8_t)); //on prend pour taille ça car c'est comme ça
    uint16_t nombre_pour_calcul = 0;
    for (int i = 1; i <= _min_ ; i++) { //on va jusqu'a min car sinon un uint8* sera vide
        nombre_pour_calcul += _Arg1[_Size1-i] + _Arg2[_Size2-i];
        resultat[length-i] = nombre_pour_calcul & 255; //prends les 8 premiers bits
        nombre_pour_calcul = nombre_pour_calcul >> 8; //on conserve la retenue
    }
    if (arg1_bigger) {
        for (int i = _min_+1; i <= _max_; i++) {
            nombre_pour_calcul += _Arg1[_Size1-i];
            resultat[length-i] = nombre_pour_calcul & 255;
            nombre_pour_calcul = nombre_pour_calcul >> 8;
        }
        resultat[0] = nombre_pour_calcul;
    }
    else {
        for (int i = _min_+1; i <= _max_; i++) {
            nombre_pour_calcul += _Arg2[_Size2-i];
            resultat[length-i] = nombre_pour_calcul & 255;
            nombre_pour_calcul = nombre_pour_calcul >> 8; 
        }
        resultat[0] = nombre_pour_calcul;
    }
    
    //recheck length (pour éviter d'utiliser de la mémoire pour rien)
    uint64_t retest = 0;
    for (int i = 0; resultat[i] == 0; i++) {
        retest++;
    }
    if (retest != 0) {
        length -= retest;
        for (int i = 0; i < length ; i++) {
            resultat[i] = resultat[i+retest];
        }
    }
    //(oui évidemment ça sert à rien sans realloc)
    resultat = (uint8_t*) realloc(resultat, length);
    if (_RSize != NULL) {
        (*_RSize) = length;
    }

    return resultat;
}

/* ----------------------------------------------- */

char* convert_to_str(uint8_t* from_, int size) {
    char* returned = (char*)(calloc(size*2+2,sizeof(char)));
    if (returned == NULL) {
        exit(47);
    }
    returned[0] = '0'; //le "0x" au début n'est pas obligatoire, mais je l'aime bien là où il est
    returned[1] = 'x';
    int a = size-1;
    char constants[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'}; //ça évite d'avoir 16 if/else
    for (int i = 0; i <= a; i++) { //on commence du début car little endian
        uint8_t visualisation = (uint8_t) from_[i];
        returned[i*2+2] = constants[visualisation/16]; //prends les 4 bits du haut (1010 0101 -> 1010)
        returned[i*2+3] = constants[visualisation & 15]; //prends les 4 bits du bas (1010 0101 -> 0101) (grâce au & et parce que 15 c'est 0000 1111)
    }
    returned[size * 2 + 2] = '\0';
    
    return returned;
}

int main()
{
    char a[1001] = "112"; //input, peut mettre n'importe quel nombre base 10
    char b[1001] = "41456492"; //pareil
    
    timespec my_time = {0, 0};
    timespec new_time = {0, 0};
    if (timespec_get(&my_time, TIME_UTC) == NULL) {
        exit(61);
    }

    int a_len = 0;
    int b_len = 0;
    uint8_t* a_conv = conv(a,&a_len);
    uint8_t* b_conv = conv(b,&b_len);
    char* a_str = convert_to_str(a_conv, a_len);
    char* b_str = convert_to_str(b_conv,b_len);
    int c_len = 0;
    uint8_t* c_conv = addition_uint(a_conv,a_len,b_conv,b_len,&c_len);
    char* c_str = convert_to_str(c_conv,c_len);

    if (timespec_get(&new_time, TIME_UTC) == NULL) {
        exit(60);
    }

    //calcul du temps que ça à pris (simple soustraction)
    long final_time = new_time.tv_sec-my_time.tv_sec ;
    long final_time_2 = new_time.tv_nsec-my_time.tv_nsec;
    printf("nombre a : %s\n", a_str);
    printf("\n");
    printf("nombre b : %s\n",b_str);
    printf("\n");
    printf("addition a+b : %s\n",c_str);
    printf("\n");
    if (final_time_2 < 0) {
        final_time -= 1;
        final_time_2 += 1000000000 ;
    }
    printf("final_time : %ld.%09ld",final_time,final_time_2);
    printf("\nin microseconds : %ld micro seconds", final_time * 1000000 + final_time_2 / 1000);
    return 0;
}

