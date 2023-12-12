#include <iostream>
#include <string>

using namespace std;

enum Poliklinik
{
    UMUM,
    MATA,
    GIGI,
    THT,
    AFK
};

struct poliklinikStatus
{
    Poliklinik poliklinik;
    bool isReady;
};

struct Dokter
{
    string nama;
    Poliklinik poliklinik;
    string username;
    string password;
};

Poliklinik convertStringToPoliklinik(string &str)
{
    for (char &c : str)
    {
        c = toupper(c);
    }
    if (str == "UMUM")
        return UMUM;
    else if (str == "GIGI")
        return GIGI;
    else if (str == "MATA")
        return MATA;
    else if (str == "THT")
        return THT;
    else
        return AFK;
}

string convertPoliklinikToString(Poliklinik poliklinik)
{
    switch (poliklinik)
    {
    case UMUM:
        return "Umum";
        break;
    case GIGI:
        return "Gigi";
        break;
    case MATA:
        return "Mata";
        break;
    case THT:
        return "THT";
        break;
    default:
        return "Unknown UserType";
    }
}