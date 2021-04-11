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
