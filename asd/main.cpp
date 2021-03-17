#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;

int H[8][16] =
{{1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
 {0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0},
 {0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0},
 {1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
 {1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
 {0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0},
 {1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0},
 {1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1}};

string kodowanie(char msg);
char odkodowanie(int *T);
void jeden_blad(int *T, int *E);
void dwa_blad(int *T, int *E);

char BitsToChar(int *tab);

int main()
{
    string wybor = "2";
    cout << "1. Kodowanie" << endl;
    cout << "2. Odkodowywanie" << endl;
    cin >> wybor;

    string file_name_in, file_name_out;
    cout << "Podaj nazwe pliku wejsciowego:\n>";
    cin >> file_name_in;
    cout << "Podaj nazwe pliku wyjsciowego:\n>";
    cin >> file_name_out;
    if(wybor == "1")
    {
        ifstream file_we(file_name_in.c_str());
        ofstream file_wy(file_name_out.c_str());

        char znak;
        while(file_we.get(znak))
        {
            file_wy << kodowanie(znak) << endl;
        }

        file_wy.close();
        file_we.close();
    }
    else if(wybor == "2")
    {
        string line, result;
        int bit_line[16];
        ifstream file_we(file_name_in.c_str());
        char znak;
        while(!file_we.eof())
        {
            line.clear();
            getline(file_we, line);
            if(line == "\0") break;
            for(int i=0; i<16; i++)
                bit_line[i] = (int)line[i]-48;
            result += odkodowanie(bit_line);
        }
        file_we.close();

        ofstream file_wy(file_name_out.c_str());
        file_wy << result;
        file_wy.close();
    }
    else
    {
        cout << "Nieznany wybor" << endl;
        return 1;
    }

    return 0;
}

string kodowanie(char znak)
{
    string bin = bitset<8>(znak).to_string();
    int T[16];
    for(int i=0; i<16; i++)
    {
        if(i < 8)
            T[i] = bin[i]-48;
        else
            T[i] = 0;
    }

    for(int i=0; i<8; i++)
    {
        int tmp_w=0;
        for(int j=0; j<8; j++)
        {
            tmp_w += (H[i][j] * T[j]);
        }
        T[8+i] = tmp_w%2;
    }

    string res;
    for(int i=0; i<16; i++)
    {
        res += char(T[i]+48);
    }
    return res;
}

char odkodowanie(int *T)
{
    int E[8] = {0,0,0,0,0,0,0,0};

    for(int i=0; i<8; i++)
    {
        int tmp_w = 0;
        for(int j=0; j<16; j++)
        {
            tmp_w += H[i][j] * T[j];
        }
        E[i] = tmp_w%2;
    }

    for(int i=0; i<8; i++)
    {
         if(E[i] != 0)
             break;

         if(i == 7)
         {

         }
    }

    jeden_blad(T, E);
    dwa_blad(T, E);

    return BitsToChar(T);
}

void jeden_blad(int *T, int *E)
{
    for(int j=0; j<16; j++)
    {
        for(int i=0; i<8; i++)
        {
            if(E[i] != H[i][j])
                break;

            if(i == 7)
                T[j] = (T[j]+1)%2;
        }
    }
}

void dwa_blad(int *T, int *E)
{
    for(int i=0; i<15; i++)
    {
        for(int j=i+1; j<16; j++)
        {
            for(int k=0; k<8; k++)
            {
                if((H[k][i] + H[k][j])%2 != E[k])
                    break;

                if(k == 7)
                {
                    T[i] = (T[i]+1)%2;
                    T[j] = (T[j]+1)%2;
                    return;
                }
            }
        }
    }
}

char BitsToChar(int *tab)
{
    std::bitset<8> bits;
    for(int i=0; i<8; i++)
        bits[7-i] = tab[i];
    return char(bits.to_ulong());
}
