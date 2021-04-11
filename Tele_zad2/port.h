#ifndef __PORT_H__
#define __PORT_H__

HANDLE          uchwyt_portu;
LPCTSTR         numer_portu;
DCB             konfiguracja_portu;
COMSTAT         zasoby_portu;
DWORD           blad;
COMMTIMEOUTS    konfiguracja_czasu;
USHORT tmpCRC;

bool otwieraniePortu(LPCTSTR wybrany_port)
{

    uchwyt_portu = CreateFile(wybrany_port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if(uchwyt_portu != INVALID_HANDLE_VALUE)
    {
        konfiguracja_portu.DCBlength = sizeof(konfiguracja_portu);
        GetCommState(uchwyt_portu, &konfiguracja_portu);
        konfiguracja_portu.BaudRate = CBR_9600;
        konfiguracja_portu.Parity = NOPARITY;
        konfiguracja_portu.StopBits = ONESTOPBIT;
        konfiguracja_portu.ByteSize = 8;

        konfiguracja_portu.fParity = TRUE;
        konfiguracja_portu.fDtrControl = DTR_CONTROL_DISABLE;
        konfiguracja_portu.fRtsControl = RTS_CONTROL_DISABLE;
        konfiguracja_portu.fOutxCtsFlow = FALSE;
        konfiguracja_portu.fOutxDsrFlow = FALSE;
        konfiguracja_portu.fDsrSensitivity = FALSE;
        konfiguracja_portu.fAbortOnError = FALSE;
        konfiguracja_portu.fOutX = FALSE;
        konfiguracja_portu.fInX = FALSE;
        konfiguracja_portu.fErrorChar = FALSE;
        konfiguracja_portu.fNull = FALSE;

        konfiguracja_czasu.ReadIntervalTimeout = 10000;
        konfiguracja_czasu.ReadTotalTimeoutMultiplier = 10000;
        konfiguracja_czasu.ReadTotalTimeoutConstant = 10000;
        konfiguracja_czasu.WriteTotalTimeoutMultiplier = 100;
        konfiguracja_czasu.WriteTotalTimeoutConstant = 100;

        SetCommState(uchwyt_portu, &konfiguracja_portu);
        SetCommTimeouts(uchwyt_portu, &konfiguracja_czasu);
        ClearCommError(uchwyt_portu, &blad,&zasoby_portu);
        return true;
    }
    else
    {
        std::cout << "Wybrany przez ciebie port \"" << wybrany_port << "\" nie jest dostepny." << std::endl;
        return false;
    }
}

#endif //__PORT_H__
