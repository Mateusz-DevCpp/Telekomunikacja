#include "const.h"

int calcrc(char *ptr, int count)
{
    int  crc;
    char i;
    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}

int czyParzysty(int x, int y)
{
    if( y == 0 )
        return 1;
    if( y == 1 )
        return x;

    int wynik = x;
    for( int i = 2; i <= y; i++ )
        wynik = wynik * x;

    return wynik;
}

char PoliczCRC_Znaku(int n, int nrZnaku)
{
    int x, bin[16];
    for( int z = 0; z<16; z++ )
        bin[z] = 0;

    for( int i = 0; i<16; i++ )
    {
        x = n % 2;
        if( x == 1 )
            n = (n-1)/2;
        if( x == 0 )
            n = n/2;
        bin[15-i] = x;
    }

    x = 0;
    int k;

    if( nrZnaku == 1 )
        k = 7;
    if( nrZnaku == 2 )
        k = 15;

    for (int i=0; i<8; i++)
        x = x + czyParzysty(2,i) * bin[k-i];

    return (char)x;
}

int main ()
{
    system("cls");
    cout << "NADAJNIK\n";
    ifstream plik;
    char znak;
    int licznikZnakow = 1;
    unsigned long rozmiarZnaku = sizeof(znak);
    int kod;

    bool transmisja = false;
    bool czyPoprawnyPakiet;
    int nrBloku = 1;
    char blok[128];

    cout << "Wybierz numer portu:" << endl
         << "[2] - COM2\n[3] - COM3" << endl;

    switch(getchar())
    {
    case '2':
    {
        numerPortu = "COM2";
        break;
    }
    case '3':
    {
        numerPortu = "COM3";
        break;
    }
    }
    cout << numerPortu << " - Uruchamianie transferu...\n";
    if(ustawieniaPortu(numerPortu) == false )
        return 0;

    cout << "\nOczekiwanie na zgode do transmisji... ";
    for(int i=0; i<6; i++)
    {
        ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);

        if(znak=='C')
        {
            cout << "OK.\n";
            kod = 1;
            transmisja = true;
            break;
        }
        else if( znak == NAK )
        {
            cout << "Brak zgody.\n";
            kod = 2;
            transmisja = true;
            break;
        }
    }
    if(!transmisja)
        exit(1);

    plik.open("wyslana.txt",ios::binary);
    while(!plik.eof())
    {
        for( int i = 0; i < 128; i++ )
            blok[i] = (char)26;

        int w = 0;

        while( w<128 && !plik.eof() )
        {
            blok[w] = plik.get();
            if(plik.eof())
                blok[w] = (char)26;
            w++;
        }
        czyPoprawnyPakiet = false;

        while(!czyPoprawnyPakiet)
        {
            cout << "Wysylanie pakietu... ";

            cout << nrBloku;
            WriteFile(uchwytPortu, &SOH,licznikZnakow,&rozmiarZnaku, NULL);
            znak=(char)nrBloku;
            WriteFile(uchwytPortu, &znak,licznikZnakow,&rozmiarZnaku, NULL);
            znak=(char)255-nrBloku;
            WriteFile(uchwytPortu, &znak,licznikZnakow,&rozmiarZnaku, NULL);


            for( int i=0; i<128; i++ )
                WriteFile(uchwytPortu, &blok[i],licznikZnakow,&rozmiarZnaku, NULL);
            if( kod == 2 )
            {
                char suma_kontrolna=(char)26;
                for(int i=0; i<128; i++)
                    suma_kontrolna+=blok[i]%256;
                WriteFile(uchwytPortu, &suma_kontrolna,licznikZnakow,&rozmiarZnaku, NULL);
                cout<<" Suma kontrolna = " << (int)suma_kontrolna << endl;
            }
            else if(kod==1)
            {
                tmpCRC=calcrc(blok,128);
                znak=PoliczCRC_Znaku(tmpCRC,1);
                WriteFile(uchwytPortu,&znak,licznikZnakow,&rozmiarZnaku, NULL);
                znak=PoliczCRC_Znaku(tmpCRC,2);
                WriteFile(uchwytPortu,&znak,licznikZnakow,&rozmiarZnaku, NULL);
            }

            while(1)
            {
                znak=' ';
                ReadFile(uchwytPortu,&znak,licznikZnakow,&rozmiarZnaku, NULL);

                if(znak == ACK)
                {
                    czyPoprawnyPakiet=true;
                    cout<<" pakiet przeslany poprawnie.\n";
                    break;
                }

                if(znak == NAK)
                {
                    cout << "Wysylanie pakietu anulowane (NAK).\n";
                    break;
                }

                if(znak == CAN)
                {
                    cout << "Polaczenie przerwane!\n";
                    return 1;
                }
            }
        }
        if( nrBloku == 255 )
            nrBloku = 1;
        else
            nrBloku++;
    }
    plik.close();

    while(1)
    {
        znak = EOT;
        WriteFile(uchwytPortu,&znak,licznikZnakow,&rozmiarZnaku, NULL);
        ReadFile(uchwytPortu,&znak,licznikZnakow,&rozmiarZnaku, NULL);
        if(znak == ACK)
            break;
    }
    CloseHandle(uchwytPortu);
    cout << "Transfer pliku zakonczony pomyslnie :)\n";

    return 0;
}
