#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

using namespace std;

int main()
{
    char wybor;
    cout << "Co chcesz zrobic?" << endl;
    cout << "[0] Odtwarzaj nagranie" << endl;
    cout << "[1] Nagrywaj dzwiek" << endl;
    cout << ">" << endl;
    cin >> wybor;

    LPCSTR nazwa_pliku;

    switch(wybor)
    {
    case '0':
    {
        string nazwa;
        cout << "Podaj nazwe pliku\n>";
        cin >> nazwa;
        nazwa_pliku = nazwa.c_str();
        PlaySound(TEXT(nazwa_pliku), NULL, SND_ASYNC);
        system("pause");
    }
    break;
    case '1':
    {
        char ReturnString[512];
        int mci_error;

        mci_error = mciSendString("open new type waveaudio alias telekom", ReturnString, sizeof(ReturnString), NULL);
        mci_error = mciSendString("set telekom time format ms", ReturnString, sizeof(ReturnString), NULL);
        mci_error = mciSendString("record telekom notify", ReturnString, sizeof(ReturnString), NULL);
        cout << "Nagrywanie. Nacisnij dowolny przycisk, aby zakonczyc nagrywanie" << endl;
        getchar();
        getchar();

        mci_error = mciSendString("stop telekom", ReturnString, sizeof(ReturnString), NULL);
        mci_error = mciSendString("save telekom record.wav", ReturnString, sizeof(ReturnString), NULL);
        mci_error = mciSendString("close telekom", ReturnString, sizeof(ReturnString), NULL);
        cout << "Nagranie zostalo zapisane." << endl;
        cout << "Kod bledu: " << mci_error << endl;


        char czy_odtworzyc;
        cout << "Czy chcesz odtworzyc nagranie \n";
        cout << "[0] - nie\n[1] - tak\n>";
        cin >> czy_odtworzyc;
        if(czy_odtworzyc == '1')
        {
            nazwa_pliku = "record.wav";
            PlaySound(TEXT(nazwa_pliku), NULL, SND_ASYNC);
            system("pause");
        }
    }
    break;
    }

    return 0;
}
