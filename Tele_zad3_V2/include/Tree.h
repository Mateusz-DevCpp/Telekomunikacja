#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>

class Klucz
{
public:
    char znak;
    int ilosc;
    std::string code;
    Klucz(char z, int v):znak(z),ilosc(v){;}    /// to jest po to, zebym mogl dodawac wartosci do wektora
    bool operator<(Klucz &x){return this->ilosc < x.ilosc;}    /// to jest po to, zeby sort(...) dzialalo
};

class Klucze
{
public:
    static std::string MakeKey(std::string oryginalna_wiadomosc);/// to zwraca ile jakich liter jest w tekscie
    static std::vector<Klucz> getKeyList(std::string klucze_str);/// to zamienia powyzszy wynik na wektor
    static std::vector<Klucz> setCodes(std::vector<Klucz> keys); /// a to nadaje kody wartoscia w wertorze
};

#endif // TREE_H
