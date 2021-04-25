#include <iostream>
#include <conio.h>
#include <fstream>
#include <winsock2.h>

#include "KodowanieHuffmana.h"

using namespace std;

string loadFromFile(string nazwa_pliku);
bool saveToFile(string nazwa_pliku, string msg);
string makeMessage();

enum TYP_PROGRAMU
{
    TYPE_SERVER = 0,
    TYPE_CLIENT = 1
};

#define MAX_MSG_LEN 8192

int main()
{
    /*
    cout << "ala ma kota a kot ma ale" << endl;

    string zakodowana = KodowanieHuffmana::kodowanieHuffmana("ala ma kota a kot ma ale");
    cout << KodowanieHuffmana::kodowanieHuffmana("ala ma kota a kot ma ale") << endl;
    cout << KodowanieHuffmana::stringToBytes(zakodowana) << endl;

    zakodowana = KodowanieHuffmana::stringToBytes(zakodowana);
    cout << KodowanieHuffmana::bytesToString(zakodowana) << endl;
    cout << KodowanieHuffmana::dekodowanieHuffmana(KodowanieHuffmana::bytesToString(zakodowana)) << endl;
    return 0;
    */

    string ip_address = "127.0.0.1";
    unsigned int port = 57000;

    string wiadomosc;

    TYP_PROGRAMU typ_programu;
    WSADATA wsaData;
    SOCKET mainSocket;
    sockaddr_in service;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(result != NO_ERROR)
    {
        cout << "Blad inicjalizacji" << endl;
    }

    mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(mainSocket == INVALID_SOCKET)
    {
        cout << "Blad podczas tworzenia socketa " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    char tryb;
    string tmp_;
    cout << "Wlacz program w trybie:\n[s] - serwera\n[c] - klienta" << endl;
    cin >> tryb;

    if(tryb == 's')
    {
        typ_programu = TYPE_SERVER;
    }
    else
    {
        typ_programu = TYPE_CLIENT;
    }

    cout << "Address [0-local]: \n>";
    cin >> ip_address;
    if(ip_address == "0")
    {
        ip_address = "127.0.0.1";
    }

    cout << "Port [0-default]: \n>";
    cin >> port;
    if(port == 0)
        port = 57000;

    if(typ_programu == TYPE_CLIENT)
    {
        cout << "Co chcesz wyslac do serwera[nazwa pliku(0 - plik.txt)]:\n>";
        cin >> wiadomosc;

        if(wiadomosc == "0")
            wiadomosc = "plik.txt";
    }

    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ip_address.c_str());
    service.sin_port = htons(port);

    switch(typ_programu)
    {
    case TYPE_SERVER:
    {
        if(bind(mainSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
        {
            cout << "Bindowanie nie powiodlo sie" << endl;
            closesocket(mainSocket);
            return 1;
        }

        if(listen(mainSocket, 1) == SOCKET_ERROR)
        {
            cout << "Blad podczas nasluchiwania" << endl;
        }

        cout << "Oczekiwanie na klienta" << endl;
        SOCKET acceptSocket = SOCKET_ERROR;
        while(acceptSocket == SOCKET_ERROR)
        {
            acceptSocket = accept(mainSocket, NULL, NULL);
        }
        cout << "Klient polaczyl sie z serwerem" << endl;
        mainSocket = acceptSocket;

        char recvBuffor[MAX_MSG_LEN] = "";
        int recvSize = SOCKET_ERROR;

        while(recvSize == SOCKET_ERROR)
        {
            recvSize = recv(mainSocket, recvBuffor, MAX_MSG_LEN, 0);
        }
        if(recvSize == -1)
        {
            cout << "Klien zakonczyl polaczenie." << endl;
            return 0;
        }
        cout << "Odebrano " << recvSize << " bajtow." << endl;

        char co_zrobic;
        cout << "Co zrobic z otrzymana wiadomoscia:\n[0] - zapisz do pliku\n[1] - wyswietl na konsoli" << endl;
        cin >> co_zrobic;

        string mes = string(recvBuffor);
        mes = KodowanieHuffmana::dekodowanieHuffmana(KodowanieHuffmana::bytesToString(mes));

        if(co_zrobic == '0')
        {
            string nazwa_pliku;
            cout << "Podaj nazwe pliku:\n>";
            cin >> nazwa_pliku;
            saveToFile(nazwa_pliku, mes);
        }
        else
        {
            cout << mes << endl;
        }

    }
    break;
    case TYPE_CLIENT:
    {
        if(connect(mainSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
        {
            cout << "Nie udalo sie polaczyc z serwerem" << endl;
            WSACleanup();
            return 1;
        }

        wiadomosc = loadFromFile(wiadomosc);
        wiadomosc = KodowanieHuffmana::stringToBytes(KodowanieHuffmana::kodowanieHuffmana(wiadomosc));
        send(mainSocket, wiadomosc.c_str(), wiadomosc.length(), 0);
        cout << "Wyslano wiadomosc:" << endl;
        cout << wiadomosc << endl;
    }
    break;
    }

    system("pause");
    return 0;
}



string loadFromFile(string nazwa_pliku)
{
    string zaw;
    fstream plik(nazwa_pliku.c_str(), ios::in);
    if(plik.good())
    {
        string tmp;
        while(!plik.eof())
        {
            getline(plik, tmp);
            zaw += tmp + "\n";
        }
        plik.close();
    }
    else
        return "";
    zaw.erase(zaw.end()-1, zaw.end());
    return zaw;
}

bool saveToFile(string nazwa_pliku, string msg)
{
    fstream plik(nazwa_pliku.c_str(), ios::out);
    if(plik.good())
    {
        plik << msg;
        plik.close();
    }
    else
        return false;
    return true;
}
