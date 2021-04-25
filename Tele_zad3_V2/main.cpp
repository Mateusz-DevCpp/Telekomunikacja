#include <iostream>
#include <conio.h>
#include <fstream>
#include <winsock2.h>

#include "KodowanieHuffmana.h"
#include "Tree.h"

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
    for(int i=0; i<KodowanieHuffmana::klucz.size(); i++)
    {
        cout << KodowanieHuffmana::klucz[i].znak << ":" << KodowanieHuffmana::klucz[i].ilosc << ":" << KodowanieHuffmana::klucz[i].code << endl;
    }
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

    char co_wyslac;
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
        cout << "Co chcesz wyslac do serwera:\n[0] - plik\n[1] - wprowadzona wiadomosc\n>";
        cin >> co_wyslac;
        if(co_wyslac == '0')
        {
            cout << "Nazwa pliku: [0 - plik.txt]\n>";
            cin >> wiadomosc;

            if(wiadomosc == "0")
                wiadomosc = "plik.txt";
        }
        else
        {
            co_wyslac = '1';
            cout << "Wiadomosc: \n>";
            wiadomosc = makeMessage();

            cout << "Address [0-local]: \n>" << ip_address << endl;
            cout << "Port [0-default]: \n>" << port << endl;
            cout << "Co chcesz wyslac do serwera:\n[0] - plik\n[1] - wprowadzona wiadomosc\n>1"<< endl;
            cout << "Wiadomosc: \n\"\n" << wiadomosc << "\n\"" << endl << endl;
        }
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

        {
            while(recvSize == SOCKET_ERROR)
            {
                recvSize = recv(mainSocket, recvBuffor, MAX_MSG_LEN, 0);
            }
            if(recvSize == -1)
            {
                cout << "Klien zakonczyl polaczenie." << endl;
                return 0;
            }
            cout << "Odebrano klucz." << endl;
            string klucz = string(recvBuffor);
            KodowanieHuffmana::klucz = Klucze::getKeyList(klucz);
            KodowanieHuffmana::klucz = Klucze::setCodes(KodowanieHuffmana::klucz);
        }///odbieranie klucza do dekompresji wiadomosci

        for(int i=0; i<MAX_MSG_LEN; i++)
            recvBuffor[i] = 0;

        recvSize = SOCKET_ERROR;

        {
            while(recvSize == SOCKET_ERROR)
            {
                recvSize = recv(mainSocket, recvBuffor, MAX_MSG_LEN, 0);
            }
            if(recvSize == -1)
            {
                cout << "Klien zakonczyl polaczenie." << endl;
                return 0;
            }

            string mes = string(recvBuffor);

            char co_zrobic;
            cout << "Co zrobic z otrzymana wiadomoscia:\n[0] - zapisz do pliku\n[1] - wyswietl na konsoli" << endl;
            cin >> co_zrobic;
            if(co_zrobic == '0')
            {
                mes = KodowanieHuffmana::dekodowanieHuffmana( KodowanieHuffmana::bytesToString(mes) );
                string nazwa_pliku;
                cout << "Podaj nazwe pliku:\n>";
                cin >> nazwa_pliku;
                saveToFile(nazwa_pliku, mes);
            }
            else
            {
                cout << "odebrana wiadomosc: " << endl << mes << endl;
                mes = KodowanieHuffmana::bytesToString(mes);
                cout << "odebrana wiadomosc binarnie: " << endl << mes << endl;
                mes = KodowanieHuffmana::dekodowanieHuffmana(mes);
                cout << "W wersji zdekompresowanej: " << endl << mes << endl;
            }
        }///odbieranie wiadomosci.
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

        if(co_wyslac == '0')
        {
            wiadomosc = loadFromFile(wiadomosc);
        }

        KodowanieHuffmana::klucz = Klucze::getKeyList( Klucze::MakeKey(wiadomosc) );
        KodowanieHuffmana::klucz = Klucze::setCodes(KodowanieHuffmana::klucz);
        send(mainSocket, Klucze::MakeKey(wiadomosc).c_str(), Klucze::MakeKey(wiadomosc).length(), 0);
        cout << "Wyslano klucz kompresji dla wiadomosc." << endl;

        cout << "Wiadomosc to: " << endl << wiadomosc << endl;
        wiadomosc = KodowanieHuffmana::kodowanieHuffmana(wiadomosc);
        wiadomosc = KodowanieHuffmana::stringToBytes(wiadomosc);
        send(mainSocket, wiadomosc.c_str(), wiadomosc.length(), 0);
        cout << "Wyslano skompresowana wiadomosc:" << endl << wiadomosc << endl;
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

string makeMessage()
{
    string msg = "";
    for(;;)
    {
        system("cls");
        cout << "Mozesz pisac, gdy napiszesz wiadomosc nacisnij ESC." << endl;
        cout << msg;

        char znak = getch();

        if(znak == char(27))
        {
            break;
        }
        else if(znak == char(13))
        {
            msg += "\n";
            cout << endl;
        }
        else if(znak == char(8))
        {
            if(msg.size() > 0)
                msg.erase(msg.size()-1);
        }
        else
        {
            msg += znak;
        }
    }
    system("cls");
    return msg;
}
