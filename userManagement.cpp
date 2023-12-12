#include <iostream>
#include <string>

using namespace std;

enum UserType
{
    ADMIN,
    DOKTER,
    SUSTER,
    KASIR,
    PASIEN
};

struct UserData
{
    string username;
    string password;
    UserType usertype;
};

UserType convertStringToUserType(string &str)
{
    for (char &c : str)
    {
        c = toupper(c);
    }
    if (str == "ADMIN")
        return ADMIN;
    else if (str == "DOKTER")
        return DOKTER;
    else if (str == "SUSTER")
        return SUSTER;
    else if (str == "KASIR")
        return KASIR;
    else
        return PASIEN;
}

string convertUserTypeToString(UserType userType)
{
    switch (userType)
    {
    case ADMIN:
        return "Admin";
        break;
    case DOKTER:
        return "Dokter";
        break;
    case SUSTER:
        return "Suster";
        break;
    case KASIR:
        return "Kasir";
        break;
    case PASIEN:
        return "Pasien";
        break;
    default:
        return "Unknown UserType";
    }
}