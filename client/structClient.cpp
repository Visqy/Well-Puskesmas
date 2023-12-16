#include <string>
#include <sstream>
#include <iomanip>
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

typedef struct treatment *Treatment;
typedef struct diagnosa *Diagnosa;

struct TensiDarah
{
    int sistolik;
    int diastolik;
};

struct pemeriksaanAwal
{
    int beratBadan;
    int tinggiBadan;
    int suhuBadan;
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

struct Pasien
{
    int nomor;
    dataPribadi dataPribadi;
    pemeriksaanAwal pemeriksaanAwal;
    Diagnosa diagnosaPasien;
};

struct riwayatTransaksi
{
    int nomorTransaksi, idPasien, idDiagnosa;
    string waktuTransaksi, namaPasien;
    treatment *treatmentPasien;
    long total, jumlahDibayarkan;
};

typedef vector<Pasien> pasienArray;
typedef vector<riwayatTransaksi> riwayatTransaksiArray;

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

typedef vector<Dokter> dokterArray;
typedef vector<UserData> userdataArray;

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

void insertLastTreatment(Treatment &first, Treatment pBantu)
{
    if (first == NULL)
    {
        first = pBantu;
    }
    else
    {
        Treatment currentTreatment = first;
        while (currentTreatment->next != NULL)
        {
            currentTreatment = currentTreatment->next;
        }
        currentTreatment->next = pBantu;
    }
}

string createStringFromDataPribadi(dataPribadi &dataPribadiPasien)
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

string createStringFromPemeriksaanAwal(pemeriksaanAwal &dataPemeriksaanAwal)
{
    ostringstream oss;
    oss << dataPemeriksaanAwal.beratBadan << "#";
    oss << dataPemeriksaanAwal.tinggiBadan << "#";
    oss << dataPemeriksaanAwal.suhuBadan << "#";
    oss << dataPemeriksaanAwal.tekananDarah.sistolik << "#";
    oss << dataPemeriksaanAwal.tekananDarah.diastolik;

    return oss.str();
}

pemeriksaanAwal createPemeriksaanAwalFromString(string &input)
{
    pemeriksaanAwal dataPemeriksaanAwal;
    istringstream iss(input);

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

dataPribadi createDataPribadiFromString(string &input)
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

Diagnosa createDiagnosaFromString(string &diagnosaString)
{
    Diagnosa diagnosaData;
    stringstream ss(diagnosaString);
    string token;

    // Mendapatkan nilai untuk atribut id
    getline(ss, token, '#');
    diagnosaData->id = stoi(token);

    // Mendapatkan nilai untuk atribut hasilDiagnosa
    getline(ss, token, '#');
    diagnosaData->hasilDiagnosa = token;

    // Mendapatkan nilai untuk atribut waktuDiagnosa
    getline(ss, token, '#');
    diagnosaData->waktuDiagnosa = token;

    // Mendapatkan nilai untuk atribut Treatment jika ada
    while (getline(ss, token, '#'))
    {
        Treatment newTreatment = new treatment;
        newTreatment->id = stoi(token);

        getline(ss, token, '#');
        newTreatment->namaTreatment = token;

        getline(ss, token, '#');
        newTreatment->biaya = stoi(token);

        newTreatment->next = NULL;

        // Menambahkan Treatment ke dalam diagnosaData
        insertLastTreatment(diagnosaData->treatmentDiagnosa, newTreatment);
    }

    return diagnosaData;
}

void displayOutputPemeriksaan(pemeriksaanAwal data)
{
    cout << "\nData Pemeriksaan Awal" << endl;
    cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;
    cout << setw(15) << left << "|Berat Badan"
         << setw(15) << "|Tinggi Badan"
         << setw(15) << "|Suhu Badan"
         << setw(15) << "|Tensi Darah" << endl;
    cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;
    cout << setw(15) << "|" << setw(15) << "|" << setw(15) << "|"
         << setw(15) << "|Sistolik"
         << setw(15) << "|Diastolik" << endl;
    cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;
    cout << "|" << setw(14) << data.beratBadan << "|" << setw(14) << data.tinggiBadan
         << "|" << setw(14) << data.suhuBadan << "|" << setw(14) << data.tekananDarah.sistolik
         << "|" << setw(14) << data.tekananDarah.diastolik << endl;
    cout << setfill('-') << setw(80) << "" << setfill(' ') << endl
         << endl;
}

void printDataPribadi(const dataPribadi &data)
{
    cout << "\nInformasi Pasien" << endl;
    cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;
    cout << setw(15) << left << "Nama:" << data.nama << endl;
    cout << setw(15) << left << "Alamat:" << data.alamat << endl;
    cout << setw(15) << left << "Jenis Kelamin:" << data.jenisKelamin << endl;
    cout << setw(15) << left << "Tempat Lahir:" << data.tempatLahir << endl;
    cout << setw(15) << left << "Tanggal Lahir:" << data.tanggalLahir << endl;
    cout << setw(15) << left << "Kontak:" << data.kontak << endl;
    cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;
}

void cetakNota(riwayatTransaksi rt, string dokter, string poliklinik)
{
    // Informasi Puskesmas
    cout << "NOTA PEMERIKSAAN KESEHATAN\n\n";
    cout << "WELL PUSKESMAS\n";
    cout << "------------------------------\n";
    cout << "Jalan Raya Bandung - Sumedang KM. 21 Jatinangor 45361, Sumedang, Jawa Barat\n\n";

    // Informasi pasien (harap disesuaikan dengan data pasien yang sebenarnya)
    cout << "No. Pasien       : " << rt.idPasien << endl;
    cout << "Nama Pasien      : " << rt.namaPasien << endl;
    cout << "Waktu Periksa    : " << rt.waktuTransaksi << endl;
    cout << "Poliklinik       : " << poliklinik << endl;
    cout << "Dokter Pemeriksa ; " << dokter << endl
         << endl;

    cout << setw(60) << "Waktu Cetak : " << rt.waktuTransaksi << endl
         << endl;

    // Daftar Tindakan Pasien
    cout << "\nDAFTAR TINDAKAN PASIEN\n";
    cout << "============================================================================================\n";
    cout << setw(30) << "TINDAKAN" << setw(20) << "BIAYA\n";
    cout << "============================================================================================\n";

    Treatment pBantu = rt.treatmentPasien;
    while (pBantu != NULL)
    {
        cout << setw(30) << left << pBantu->namaTreatment;
        cout << setw(20) << left << pBantu->biaya << endl;
        pBantu = pBantu->next;
    }

    cout << "============================================================================================\n";
    cout << setw(65) << "TOTAL TINDAKAN (Rp) : " << rt.total << endl;
    cout << "\n";

    cout << "--------------------------------------------------------------------------------------------\n";
    cout << setw(28) << "TOTAL TINDAKAN (Rp) : " << rt.total << endl;
    cout << setw(28) << "UANG MUKA      (Rp) : " << rt.jumlahDibayarkan << endl;
    cout << "================================ (-)\n";
    cout << setw(28) << "UANG KEMBALI   (Rp) : " << rt.total - rt.jumlahDibayarkan << endl;
    cout << "--------------------------------------------------------------------------------------------\n";
}

void printTreatment(Treatment treatmentPasien)
{
    cout << "\nDAFTAR TINDAKAN PASIEN\n";
    cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;
    cout << setw(30) << "TINDAKAN" << setw(20) << "BIAYA\n";
    cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;

    Treatment pBantu = treatmentPasien;
    while (pBantu != NULL)
    {
        cout << setw(30) << left << pBantu->namaTreatment;
        cout << setw(20) << left << pBantu->biaya << endl;
        pBantu = pBantu->next;
    }

    cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;
}