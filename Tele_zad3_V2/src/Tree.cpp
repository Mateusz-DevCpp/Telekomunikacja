#include "Tree.h"

#include <cmath>
int StringToInt(std::string liczba)
{
    int x = 0;
    for(int i=liczba.size()-1; i>=0; i--)
    {
        x += (liczba[i]-'0')*pow(10,i);
    }
    return x;
}

std::string Klucze::MakeKey(std::string oryginalna_wiadomosc)
{
    std::stringstream ss;
    std::map<char, int> mapa_znakow;
    for(auto &z: oryginalna_wiadomosc)
    {
        mapa_znakow[z]++;
    }

    for(char x=-128; x<127; x++)
    {
        if(mapa_znakow[x] != 0)
        {
            ss << x << std::endl << mapa_znakow[x] << std::endl;
        }
    }
    ss << '$' << std::endl;
    ss << 0;
    return ss.str();
}

std::vector<Klucz> Klucze::getKeyList(std::string klucze_str)
{
    std::vector<Klucz> keys;

    std::fstream plik("tmp.file", std::ios::out);
    plik << klucze_str;
    plik.close();

    std::string tmp_c, tmp_i_c;
    int tmp_i;
    bool znak = true;
    plik.open("tmp.file", std::ios::in);
    while(!plik.eof())
    {
        if(znak)
        {
            std::getline(plik, tmp_c);
        }
        else
        {
            std::getline(plik, tmp_i_c);
            tmp_i = StringToInt(tmp_i_c);
            keys.push_back(Klucz(tmp_c[0], tmp_i));
        }
        znak = !znak;
    }
    plik.close();

    remove("tmp.file");
    return keys;
}

std::vector<Klucz> Klucze::setCodes(std::vector<Klucz> keys)
{
    std::sort(keys.begin(), keys.end());
    for(int i=keys.size()-1; i>=0; i--)
    {
        for(int j=1; j<keys.size()-i; j++)
        {
            keys[i].code += '1';
        }
        if(i>0)
            keys[i].code += '0';
    }
    return keys;
}
