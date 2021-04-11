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
    unsigned long rozmiar_znaku = sizeof(znak_sterujacy);
    int licznik_znakow = 1;

    cout << "ODBIORNIK\n";
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
        return 0;
    }
    getchar();

    cout << "Zgoda na transfer pliku\n[1] - tak (ACK)\n[2] - nie (NAK)" << endl;
    char decyzja = getchar();
    if(decyzja == '1')
    {
        znak_sterujacy = C;
    }
    else if(decyzja == '2')
    {
        znak_sterujacy = NAK;
    }
    else
    {
        znak_sterujacy = NAK;
    }

    bool transmisja = false;
    for(int i=0; i<6; i++)
    {
        WriteFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);
        ReadFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);
        if(znak_sterujacy == SOH)
        {
            cout << "Polaczenie powiodlo sie" << endl;
            transmisja = true;
            break;
        }
    }
    if(!transmisja)
    {
        cout << "Polaczenie nie powiodlo sie." << endl;
        return 1;
    }

    ofstream plik("odebrana.txt",ios::binary);

    int nr_bloku = 0;
    char blok_danych[128];
    char suma_kontrolna_CRC[2];
    for(int i=0;;i++)
    {
        if(i > 0)
        {
            ReadFile(uchwyt_portu, &znak_sterujacy,licznik_znakow,&rozmiar_znaku, NULL);
            if(znak_sterujacy == EOT || znak_sterujacy == CAN)
            {
                break;
            }
            cout << endl << "Odbieranie pakietu... " << nr_bloku+1 << endl;
        }

        ReadFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);
        nr_bloku = (int)znak_sterujacy;
        ReadFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);
        char dopelnienie = znak_sterujacy;

        for(int i=0; i<128; i++)
        {
            ReadFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);
            blok_danych[i] = znak_sterujacy;
        }

        ReadFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);
        suma_kontrolna_CRC[0] = znak_sterujacy;
        ReadFile(uchwyt_portu, &znak_sterujacy, licznik_znakow, &rozmiar_znaku, NULL);
        suma_kontrolna_CRC[1] = znak_sterujacy;
        bool poprawny_pakiet = true;

        if((char)(255-nr_bloku) != dopelnienie)
        {
            cout << "Niepoprawny numer pakietu." << endl;
            WriteFile(uchwyt_portu, &NAK, licznik_znakow, &rozmiar_znaku, NULL);
            poprawny_pakiet = false;
        }
        else
        {
            tmpCRC = calcrc(blok_danych,128);

            if(PoliczCRC_Znaku(tmpCRC, 1) != suma_kontrolna_CRC[0]
               || PoliczCRC_Znaku(tmpCRC, 2) != suma_kontrolna_CRC[1])
            {
                cout << "Nieprawidlowa suma kontrolna." << endl;
                WriteFile(uchwyt_portu, &NAK, licznik_znakow, &rozmiar_znaku, NULL);
                poprawny_pakiet = false;
            }
        }
        if(poprawny_pakiet)
        {
            for(int i=0; i<128; i++)
            {
                if(blok_danych[i]!=26)
                    plik<<blok_danych[i];
            }

            cout << "Pakiet zostal odebrany poprawnie." << endl;
            WriteFile(uchwyt_portu, &ACK, licznik_znakow, &rozmiar_znaku, NULL);
        }
    }
    WriteFile(uchwyt_portu, &ACK, licznik_znakow, &rozmiar_znaku, NULL);

    plik.close();

    CloseHandle(uchwyt_portu);

    if(znak_sterujacy == CAN)
    {
        cout << "Polaczenie zostalo przerwane." << endl;
    }
    else
    {
        cout << "Plik zostal przeslany pomyslnie" << endl;
    }

    return 0;
}
