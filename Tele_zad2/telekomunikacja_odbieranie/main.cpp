#include "const.h"
using namespace std;

ofstream plik;

char blokDanych[128];
char znak;
unsigned long rozmiarZnaku= sizeof(znak);
int licznikZnakow=1;
bool transmisja=false;
bool poprawnyPakiet;
int nrBloku;
char dopelnienieDo255;
char sumaKontrolnaCRC[2];

int PoliczCRC(char *wsk, int count)
{
    int sumaKontrolnaCRC = 0;
    while (--count >= 0)
    {
        sumaKontrolnaCRC = sumaKontrolnaCRC ^ (int)*wsk++ << 8;

        for (int i = 0; i < 8; ++i)
            if (sumaKontrolnaCRC & 0x8000)
                sumaKontrolnaCRC = sumaKontrolnaCRC << 1 ^ 0x1021;
            else
                sumaKontrolnaCRC = sumaKontrolnaCRC << 1;
    }
    return (sumaKontrolnaCRC & 0xFFFF);
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
    int x, binarna[16];

    for(int z=0; z<16; z++)
        binarna[z]=0;

    for(int i=0; i<16; i++)
    {
        x=n%2;
        if (x==1)
            n=(n-1)/2;
        if (x==0)
            n=n/2;
        binarna[15-i]=x;
    }

    x=0;
    int k;

    if(nrZnaku==1)
        k=7;
    if(nrZnaku==2)
        k=15;

    for (int i=0; i<8; i++)
        x=x+czyParzysty(2,i)*binarna[k-i];

    return (char)x;
}

int main()
{
    system("cls");
    cout << "ODBIORNIK\n";

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
    getchar();

    cout << "Zgoda na transfer pliku" << endl
         << "[1] - tak (ACK)" << endl
         << "[2] - nie (NAK)" << endl;

    switch(getchar())
    {
    case '1':
    {
        znak = 'C';
        break;
    }
    case '2':
    {
        znak = NAK;
        break;
    }
    default:
    {
        cout << "Ustawienie domyslne: ACK.\n";
        znak = 'C';
    }
    }

    for(int i=0; i<6; i++)
    {
        WriteFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
        cout<<"Oczekiwanie na komunikat SOH...\n";
        ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
        cout << (int)znak << endl;
        if(znak==SOH)
        {
            cout << "Ustanowienie polaczenia powiodlo sie!\n";
            transmisja = true;
            break;
        }
    }

    if(!transmisja)
    {
        cout << "Polaczenie nieudane.\n";
        exit(1);
    }
    plik.open("odebrana.txt",ios::binary);
    cout<<"Trwa odbieranie pliku, prosze czekac...\n";

    ReadFile(uchwytPortu, &znak,licznikZnakow,&rozmiarZnaku, NULL);
    nrBloku=(int)znak;

    ReadFile(uchwytPortu, &znak,licznikZnakow,&rozmiarZnaku, NULL);
    dopelnienieDo255=znak;

    for(int i=0; i<128; i++)
    {
        ReadFile(uchwytPortu, &znak,licznikZnakow,&rozmiarZnaku, NULL);
        blokDanych[i] = znak;
    }

    ReadFile(uchwytPortu, &znak,licznikZnakow,&rozmiarZnaku, NULL);
    sumaKontrolnaCRC[0]=znak;
    ReadFile(uchwytPortu, &znak,licznikZnakow,&rozmiarZnaku, NULL);
    sumaKontrolnaCRC[1]=znak;
    poprawnyPakiet=true;


    if ((char)(255-nrBloku)!=dopelnienieDo255)
    {
        cout << "Niepoprawny numer pakietu.\n";
        WriteFile(uchwytPortu, &NAK,licznikZnakow,&rozmiarZnaku, NULL);
        poprawnyPakiet = false;
    }
    else
    {
        tmpCRC=PoliczCRC(blokDanych,128);

        if(PoliczCRC_Znaku(tmpCRC,1) != sumaKontrolnaCRC[0] || PoliczCRC_Znaku(tmpCRC,2) != sumaKontrolnaCRC[1])
        {
            cout << "Nieprawidlowa suma kontrolna.\n";
            WriteFile(uchwytPortu, &NAK,licznikZnakow,&rozmiarZnaku, NULL);
            poprawnyPakiet=false;
        }
    }

    if(poprawnyPakiet)
    {
        for(int i=0; i<128; i++)
        {
            if(blokDanych[i]!=26)
                plik<<blokDanych[i];
        }
        cout << "Odbieranie pakietu..." << nrBloku;
        cout << " pakiet odebrany poprawnie.\n";
        WriteFile(uchwytPortu, &ACK,licznikZnakow,&rozmiarZnaku, NULL);
    }

    while(1)
    {
        ReadFile(uchwytPortu, &znak,licznikZnakow,&rozmiarZnaku, NULL);
        if(znak==EOT || znak==CAN)
            break;
        cout << "Odbieranie pakietu...";
        cout << nrBloku+1 ;

        ReadFile(uchwytPortu, &znak,licznikZnakow,&rozmiarZnaku, NULL);
        nrBloku=(int)znak;

        ReadFile(uchwytPortu, &znak,licznikZnakow,&rozmiarZnaku, NULL);
        dopelnienieDo255=znak;

        for(int i=0; i<128; i++)
        {
            ReadFile(uchwytPortu, &znak,licznikZnakow,&rozmiarZnaku, NULL);
            blokDanych[i] = znak;
        }


        ReadFile(uchwytPortu, &znak,licznikZnakow,&rozmiarZnaku, NULL);
        sumaKontrolnaCRC[0]=znak;
        ReadFile(uchwytPortu, &znak,licznikZnakow,&rozmiarZnaku, NULL);
        sumaKontrolnaCRC[1]=znak;
        poprawnyPakiet=true;

        if((char)(255-nrBloku)!=dopelnienieDo255)
        {
            cout << "Niepoprawny numer pakietu.\n";
            WriteFile(uchwytPortu, &NAK,licznikZnakow,&rozmiarZnaku, NULL);
            poprawnyPakiet=false;
        }
        else
        {
            tmpCRC=PoliczCRC(blokDanych,128);

            if(PoliczCRC_Znaku(tmpCRC,1)!=sumaKontrolnaCRC[0] || PoliczCRC_Znaku(tmpCRC,2)!=sumaKontrolnaCRC[1])
            {
                cout << "Nieprawidlowa suma kontrolna.\n";
                WriteFile(uchwytPortu, &NAK,licznikZnakow,&rozmiarZnaku, NULL);
                poprawnyPakiet=false;
            }
        }
        if(poprawnyPakiet)
        {
            for(int i=0; i<128; i++)
            {
                if(blokDanych[i]!=26)
                    plik<<blokDanych[i];
            }

            cout << " pakiet odebrany poprawnie.\n";
            WriteFile(uchwytPortu, &ACK,licznikZnakow,&rozmiarZnaku, NULL);
        }
    }
    WriteFile(uchwytPortu, &ACK,licznikZnakow,&rozmiarZnaku, NULL);

    plik.close();
    CloseHandle(uchwytPortu);

    if(znak == CAN)
        cout << "Blad transmisji - polaczenie zostalo przerwane.\n";
    else
        cout << "Transfer pliku zakonczony pomyslnie :)\n";

    return 0;
}
