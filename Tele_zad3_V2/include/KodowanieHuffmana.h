#ifndef KODOWANIEHUFFMANA_H
#define KODOWANIEHUFFMANA_H

#include <iostream>
#include <vector>
#include <string>
#include <bitset>

#include "Tree.h"

class KodowanieHuffmana
{
public:
    static std::vector<Klucz> klucz;

    static std::string getCharCode(char x);
    static char getCodeChar(std::string code);

    static std::string kodowanieHuffmana(std::string msg);
    static std::string dekodowanieHuffmana(std::string msg);

    static std::string stringToBytes(std::string zakodowany);
    static std::string bytesToString(std::string zakodowany);
};

#endif // KODOWANIEHUFFMANA_H
