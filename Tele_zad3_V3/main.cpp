#include <bits/stdc++.h>
#include <winsock2.h>

using namespace std;

const unsigned int MAX_BUF_LENGTH = 4096;

struct Node
{
    char znak;
    int czestotliwosc;
    Node *prawy, *lewy;
};

struct porownaj
{
    bool operator()(Node* l, Node* r)
    {
        return (l->czestotliwosc > r->czestotliwosc);
    }
};

Node* getNode(char znak, int czestotliwosc, Node* lewy, Node* prawy)
{
    Node* node = new Node();

    node->znak = znak;
    node->czestotliwosc = czestotliwosc;
    node->lewy = lewy;
    node->prawy = prawy;

    return node;
}

map <char, string> kod;
priority_queue<Node*, vector <Node*>, porownaj> pq;

void zapiszKody(struct Node* korzen, string str)
{
    if(korzen == nullptr)
        return;

    if(!korzen->lewy && !korzen->prawy)
        kod[korzen->znak] = str;

    zapiszKody(korzen->lewy, str + "0");
    zapiszKody(korzen->prawy, str + "1");
}

void DrzewoHuffmana(string wiadomosc)
{
    unordered_map <char, int> czestotliwosci;
    for (char ch : wiadomosc)
    {
        czestotliwosci[ch]++;
    }

    for(auto it : czestotliwosci)
    {
        pq.push(getNode(it.first, it.second, nullptr, nullptr));
    }

    while(pq.size() != 1)
    {
        Node *lewy = pq.top();
        pq.pop();
        Node *prawy = pq.top();
        pq.pop();

        int suma = lewy->czestotliwosc + prawy->czestotliwosc;
        pq.push(getNode('\0', suma, lewy, prawy));
    }

    zapiszKody(pq.top(), "");
}

string odkodowanie(string wiadomosc)
{
    string wiadomosc_odkodowana;

    vector <vector <char>> pom;
    vector <char> pom1;
    vector <char> znaki;

    for(const auto &it : kod)
    {
        int n = it.second.length();
        char char_array[n + 1];

        strcpy(char_array, it.second.c_str());

        for (int i = 0; i < n; i ++)
        {
            pom1.push_back(char_array[i]);
        }

        znaki.push_back(it.first);
        pom.push_back(pom1);
        pom1.clear();
    }

    bool flaga = false;
    for(int i = 0; i < wiadomosc.size(); i ++)
    {
        for(int j = 0; j < pom.size(); j ++)
        {
            for(int k = 0; k < pom[j].size(); k ++)
            {
                if(wiadomosc[i] == pom[j][k])
                {
                    if(k == pom[j].size() - 1)
                    {
                        wiadomosc_odkodowana += znaki[j];
                        flaga = true;
                        break;
                    }
                    i ++;
                }
                else
                {
                    i -= k;
                    break;
                }
            }
            if (flaga) break;
        }
        flaga = false;
    }

    return wiadomosc_odkodowana;
}



int main()
{
    int wybor;
    cout << "Co chcesz zrobic?" << endl;
    cout << "1 - uruchomic klienta - skompresowac i wyslac wiadomosc" << endl;
    cout << "2 - uruchomic serwer - odebrac i dekompresowac wiadomosc" << endl;
    cout << "Twoj wybor: ";
    cin >> wybor;

    cout << "Podaj adres ip [0 - localhost]:" << endl;
    string ip_adres;
    cin >> ip_adres;
    if(ip_adres == "0")
        ip_adres = "127.0.0.1";

    cout << "Podaj port [0 - domyslny]:" << endl;
    int port;
    cin >> port;
    if(port == 0)
        port = 57000;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET gniazdo = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(gniazdo == INVALID_SOCKET)
    {
        cout << "Nie udalo sie utworzyc gniazda sieciowego." << endl;
        WSAGetLastError();
        WSACleanup();
        return 1;
    }

    sockaddr_in service{};
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ip_adres.c_str());
    service.sin_port = htons(port);

    string wiadomosc, zakodowana_wiadomosc, slownik;
    string znak_wartosc = ":", kolejny_kod = ",";
    switch (wybor)
    {
    case 1:
    {
        string plik_nazwa;
        cout << "Co wyslac [0 - plik.txt]:" << endl;
        cin >> plik_nazwa;
        if(plik_nazwa == "0")
            plik_nazwa = "plik.txt";

        ifstream plik_odczyt;
        plik_odczyt.open(plik_nazwa.c_str());

        if(plik_odczyt.good())
        {
            while(!plik_odczyt.eof())
            {
                getline (plik_odczyt, wiadomosc);
            }
        }
        plik_odczyt.close();

        DrzewoHuffmana(wiadomosc);

        for(auto &it : kod)
        {
            slownik += it.first + znak_wartosc + it.second + kolejny_kod;
        }

        for(char c : wiadomosc)
        {
            zakodowana_wiadomosc += kod[c];
        }

        cout << "Odkodowana wiadomosc: " << wiadomosc << endl;
        cout << "Zakodowana wiadomosc: " << zakodowana_wiadomosc << endl;

        if(connect(gniazdo,(SOCKADDR*) &service, sizeof(service)) != SOCKET_ERROR)
        {
            cout << "Ustanawianie polaczenia." << endl;
        }
        else
        {
            cout << "Nie udalo sie ustanowic polaczenia." << endl;
            WSACleanup();
            return 1;
        }
        zakodowana_wiadomosc = slownik + ".." + zakodowana_wiadomosc + "..";

        send(gniazdo, zakodowana_wiadomosc.c_str(), zakodowana_wiadomosc.size(),0 );
        cout << "Wiadomosc zostala wyslana." << endl;
        break;
    }
    case 2:
    {
        if(bind(gniazdo, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR)
        {
            cout << "Nie udalo sie skonfigurowac gniazda." << endl;
            closesocket(gniazdo);
            return 1;
        }
        if(listen(gniazdo, 1) == SOCKET_ERROR)
        {
            cout << "Blad gniazda" << endl;
        }
        SOCKET acceptSocket = SOCKET_ERROR;
        cout << "Oczekiwanie na polaczenie" << endl;
        while(acceptSocket == SOCKET_ERROR)
        {
            acceptSocket = accept( gniazdo, NULL, NULL );
        }
        cout << "Ustanowiono polaczenie" << endl;
        gniazdo = acceptSocket;

        const unsigned int MAX_BUF_LENGTH = 4096;
        vector<char> buffer(MAX_BUF_LENGTH);
        string wiadomosc;
        int bytesReceived = 0;
        do
        {
            bytesReceived = recv(gniazdo, &buffer[0], buffer.size(), 0);
            if(bytesReceived == -1)
            {
                return 5;
            }
            else
            {
                wiadomosc.append(buffer.cbegin(), buffer.cend());
            }
        }
        while(bytesReceived == MAX_BUF_LENGTH);

        string wiadomosc_zakodowana;
        bool flaga = true;
        for (int i = 0; i < wiadomosc.size() - 1; i ++)
        {
            if(wiadomosc[i] != '.' && flaga)
            {
                slownik += wiadomosc[i];
            }
            else
            {
                if(wiadomosc[i+1] == '.' && flaga)
                {
                    flaga = false;
                    i ++;
                }
                else
                {
                    if(!flaga)
                    {
                        if(wiadomosc[i] == '.')
                            break;

                        wiadomosc_zakodowana += wiadomosc[i];
                    }
                    else slownik += wiadomosc[i];
                }
            }
        }

        int j = 0;
        string pom;
        for(int i=0; i < slownik.size() - 1; i ++)
        {
            if(slownik[i] == znak_wartosc[0] && i != j)
            {
                j = i + 1;
                while(slownik[j] != kolejny_kod[0])
                {
                    pom += slownik[j];
                    j++;
                }
                kod[slownik[i - 1]] = pom;
                pom.clear();
            }
        }

        string wiadomosc_odkodowana = odkodowanie(wiadomosc_zakodowana);
        cout << "Wiadomosc odkodowana to: " << wiadomosc_odkodowana << endl;

        string plik_nazwa;
        cout << "Gdzie zapisac:" << endl;
        cin >> plik_nazwa;
        if(plik_nazwa == "0")
            plik_nazwa = "odebrane.txt";
        ofstream plik(plik_nazwa.c_str());
        plik << wiadomosc_odkodowana;
        plik.close();
        break;
    }
    }


    system( "pause" );
    return 0;
}
