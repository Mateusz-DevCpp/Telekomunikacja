#include "KodowanieHuffmana.h"

std::vector<Klucz> KodowanieHuffmana::klucz;

std::string KodowanieHuffmana::kodowanieHuffmana(std::string msg)
{
    std::string zakodowana_wiadomosc = "";
    for(int i=0; i<msg.size(); i++)
    {
        zakodowana_wiadomosc += getCharCode(msg[i]);
    }
    zakodowana_wiadomosc += getCharCode('$');

    while(zakodowana_wiadomosc.size()%8 != 0)
    {
        zakodowana_wiadomosc += '1';
    }

    return zakodowana_wiadomosc;
}

std::string KodowanieHuffmana::dekodowanieHuffmana(std::string msg)
{
    std::string odkodowana_wiadomosc = "";
    std::string tmp;
    for(int i=0; i<msg.size(); i++)
    {
        tmp += msg[i];
        if(tmp[tmp.size()-1] == '0')
        {
            odkodowana_wiadomosc += getCodeChar(tmp);
            tmp = "";
        }
    }
    return odkodowana_wiadomosc;
}

std::string KodowanieHuffmana::stringToBytes(std::string zakodowany)
{
    std::string result = "";
    std::bitset<8> bajt;
    for(int i=0; i<zakodowany.size()/8; i++)
    {
        for(int j=0; j<8; j++)
        {
            bajt.set(7-j, zakodowany[8*i+j]-'0');
        }
        result += char(bajt.to_ulong());
    }
    return result;
}

std::string KodowanieHuffmana::bytesToString(std::string zakodowany)
{
    std::string result = "";
    std::bitset<8> bajt;
    for(int i=0; i<zakodowany.size(); i++)
    {
        bajt = std::bitset<8>(zakodowany[i]);
        result += bajt.to_string();
    }
    return result;
}

std::string KodowanieHuffmana::getCharCode(char x)
{
    for(int i=0; i<klucz.size(); i++)
    {
        if(klucz[i].znak == x)
        {
            return klucz[i].code;
        }
    }
}

char KodowanieHuffmana::getCodeChar(std::string code)
{
    for(int i=0; i<klucz.size(); i++)
    {
        if(klucz[i].code == code)
        {
            return klucz[i].znak;
        }
    }
}
