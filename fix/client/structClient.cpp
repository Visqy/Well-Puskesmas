#include <string>
#include <sstream>
using namespace std;

enum Poliklinik
{
    UMUM,
    MATA,
    GIGI,
    THT,
    AFK
};

enum UserType
{
    ADMIN,
    DOKTER,
    SUSTER,
    KASIR,
    PASIEN
};

typedef struct riwayattransaksi *riwayatTransaksi;
typedef struct treatment *Treatment;
typedef struct diagnosa *Diagnosa;
typedef struct pasien *Pasien;

struct TensiDarah
{
    int sistolik;
    int diastolik;
};

struct pemeriksaanAwal
{
    int beratBadan;
    int tinggiBadan;
    double suhuBadan;
    TensiDarah tekananDarah;
};

struct treatment
{
    int id;
    string namaTreatment;
    long biaya;
    Treatment next;
};

struct diagnosa
{
    int id;
    string hasilDiagnosa;
    string waktuDiagnosa;
    Treatment treatmentDiagnosa;
    Diagnosa next;
};

struct dataPribadi
{
    string nama;
    string alamat;
    string jenisKelamin;
    string tempatLahir;
    string tanggalLahir;
    string kontak;
};

struct pasien
{
    int nomor;
    dataPribadi dataPribadi;
    pemeriksaanAwal pemeriksaanAwal;
    Diagnosa diagnosaPasien;
    Pasien next;
};

struct riwayattransaksi
{
    int nomorTransaksi, idPasien, idDiagnosa;
    string waktuTransaksi, namaPasien;
    treatment *treatmentPasien;
    long total, jumlahDibayarkan;
    riwayattransaksi *next;
};

struct UserData
{
    string username;
    string password;
    UserType usertype;
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

string createStringFromDataPribadi(const dataPribadi &dataPribadiPasien)
{
    ostringstream oss;
    oss << dataPribadiPasien.nama << "#"
        << dataPribadiPasien.alamat << "#"
        << dataPribadiPasien.jenisKelamin << "#"
        << dataPribadiPasien.tempatLahir << "#"
        << dataPribadiPasien.tanggalLahir << "#"
        << dataPribadiPasien.kontak;
    // tambahkan atribut lainnya sesuai kebutuhan
    return oss.str();
}

string createStringFromPemeriksaanAwal(const pemeriksaanAwal &dataPemeriksaanAwal)
{
    ostringstream oss;
    oss << dataPemeriksaanAwal.beratBadan << "#";
    oss << dataPemeriksaanAwal.tinggiBadan << "#";
    oss << dataPemeriksaanAwal.suhuBadan << "#";
    oss << dataPemeriksaanAwal.tekananDarah.sistolik << "#";
    oss << dataPemeriksaanAwal.tekananDarah.diastolik;

    return oss.str();
}

pemeriksaanAwal createPemeriksaanAwalFromString(const std::string &input)
{
    pemeriksaanAwal dataPemeriksaanAwal;
    std::istringstream iss(input);

    iss >> dataPemeriksaanAwal.beratBadan;
    iss.ignore(); // Ignore the delimiter '#'
    iss >> dataPemeriksaanAwal.tinggiBadan;
    iss.ignore();
    iss >> dataPemeriksaanAwal.suhuBadan;
    iss.ignore();
    iss >> dataPemeriksaanAwal.tekananDarah.sistolik;
    iss.ignore();
    iss >> dataPemeriksaanAwal.tekananDarah.diastolik;

    return dataPemeriksaanAwal;
}

dataPribadi createDataPribadiFromString(const string &input)
{
    dataPribadi dataPribadi;
    istringstream iss(input);

    getline(iss, dataPribadi.nama, '#');
    getline(iss, dataPribadi.alamat, '#');
    getline(iss, dataPribadi.jenisKelamin, '#');
    getline(iss, dataPribadi.tempatLahir, '#');
    getline(iss, dataPribadi.tanggalLahir, '#');
    getline(iss, dataPribadi.kontak, '#');

    return dataPribadi;
}