#include <fstream>
#include <string.h>
#include <windows.h>
#include <iostream>

#include "../code.h"
#include "../port.h"
#include "../funkcje.h"

using namespace std;

int main()
{
    char znak_sterujacy;
    int licznik_znakow = 1;
    unsigned long rozmiar_znaku = sizeof(znak_sterujacy);
    int rezultat_laczenia;
    bool transmisja = false;

    cout << "NADAJNIK" << endl;
    cout << "Wybierz numer portu:\n[2] - COM2\n[3] - COM3" << endl;
    char port_id = getchar();
    if(port_id == '2')
    {
        numer_portu = "COM2";
    }
    else if(port_id == '3')
    {
        numer_portu = "COM3";
    }

    cout << "Konfiguracja portu: " << numer_portu << " ..." << endl;
    if(otwieraniePortu(numer_portu) == false)
    {
        cout << "Konfiguracja portu: " << numer_portu << " zakonczyla sie niepowodzeniem." << endl;
        return 1;
    }
    cout << "Konfiguracja portu: " << numer_portu << " zakonczyla sie sukcesem." << endl;
    cout << "Oczekiwanie na zgode do transmisji." << endl;

    for(int i=0; i<6; i++)
    {
        ReadFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);

        if(znak_sterujacy == C)
        {
            cout << "Wyrazono zgode na transmisje." << endl;
            rezultat_laczenia = 1;
            transmisja = true;
            break;
        }
        else if(znak_sterujacy == NAK)
        {
            cout << "Brak zgody na transmisje." << endl;
            rezultat_laczenia = 2;
            transmisja = false;
            break;
        }
    }

    if(transmisja == false)
    {
        return 1;
    }

    char blok[128];
    int nr_bloku = 1;
    ifstream plik("wiadomosc_do_wyslania.txt", ios::binary);
    while(!plik.eof())
    {
        for(int i = 0; i < 128; i++)
        {
            blok[i] = (char)26;
        }

        int w = 0;
        while(w < 128 && plik.eof() == false )
        {
            blok[w] = plik.get();
            if(plik.eof())
            {
                blok[w] = (char)26;
            }
            w++;
        }

        bool przeslane_poprawnie = false;
        while(!przeslane_poprawnie)
        {
            cout << endl << "Wysylanie pakietu... " << nr_bloku << endl;

            WriteFile(uchwyt_portu, &SOH, licznik_znakow, &rozmiar_znaku, NULL);
            znak_sterujacy = (char)nr_bloku;
            WriteFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);
            znak_sterujacy = (char)(255 - nr_bloku);
            WriteFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);

            for(int i=0; i<128; i++)
            {
                WriteFile(uchwyt_portu, &blok[i], licznik_znakow, &rozmiar_znaku, NULL);
            }

            if(rezultat_laczenia == 1)
            {
                tmpCRC = calcrc(blok, 128);
                znak_sterujacy = PoliczCRC_Znaku(tmpCRC, 1);
                WriteFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);
                znak_sterujacy = PoliczCRC_Znaku(tmpCRC, 2);
                WriteFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);
            }
            else if(rezultat_laczenia == 2)
            {
                char suma_kontrolna = (char)26;
                for(int i=0; i<128; i++)
                {
                    suma_kontrolna += blok[i]%256;
                }
                WriteFile(uchwyt_portu, &suma_kontrolna, licznik_znakow, &rozmiar_znaku, NULL);
                cout<<" Suma kontrolna = " << (int)suma_kontrolna << endl;
            }

            while(1)
            {
                ReadFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);

                if(znak_sterujacy == ACK)
                {
                    przeslane_poprawnie = true;
                    cout << "Pakiet przeslany poprawnie." << endl;
                    break;
                }

                if(znak_sterujacy == NAK)
                {
                    cout << "Pakiet nie zosta³ przeslany poprawnie." << endl;
                    break;
                }

                if(znak_sterujacy == CAN)
                {
                    cout << "Polaczenie zostalo przerwane." << endl;
                    return 1;
                }
            }
        }

        if(nr_bloku == 255)
        {
            nr_bloku = 1;
        }
        else
        {
            nr_bloku++;
        }
    }
    plik.close();

    while(1)
    {
        znak_sterujacy = EOT;
        WriteFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);
        ReadFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);
        if(znak_sterujacy == ACK)
        {
            break;
        }
    }

    CloseHandle(uchwyt_portu);

    cout << "Transfer pliku zakonczony pomyslnie" << endl;

    return 0;
}
