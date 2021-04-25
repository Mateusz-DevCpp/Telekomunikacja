#include "KodowanieHuffmana.h"

std::string KodowanieHuffmana::kodowanieHuffmana(std::string msg)
{
    std::string zakodowana_wiadomosc = "";
    for(int i=0; i<msg.size(); i++)
    {
        zakodowana_wiadomosc += getCharCode(msg[i]);
    }
    zakodowana_wiadomosc += getCharCode('\0');

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
        else if(tmp.size() == 8)
            break;
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
    switch(x)
    {
        case 'a': return "0";
        case ' ': return "10";
        case 't': return "110";
        case 'o': return "1110";
        case 'm': return "11110";
        case 'l': return "111110";
        case 'k': return "1111110";
        case 'e': return "11111110";
        case '\0': return "11111111";
    }
}

char KodowanieHuffmana::getCodeChar(std::string code)
{
    if(code == "0")return 'a';
    else if(code == "10") return ' ';
    else if(code == "110") return 't';
    else if(code == "1110") return 'o';
    else if(code == "11110") return 'm';
    else if(code == "111110") return 'l';
    else if(code == "1111110") return 'k';
    else if(code == "11111110") return 'e';
    else if(code == "11111111") return '\0';
}
