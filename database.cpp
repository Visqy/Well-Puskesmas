#include <iostream>
#include <sqlite3.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

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
// Insert last
void insertLastDiagnosa(Diagnosa &first, Diagnosa pBantu)
{
    if (first == NULL)
    {
        first = pBantu;
    }
    else
    {
        Diagnosa currentDiagnosa = first;
        while (currentDiagnosa->next != NULL)
        {
            currentDiagnosa = currentDiagnosa->next;
        }
        currentDiagnosa->next = pBantu;
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

// convert any struct to string for transfer data
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

// Bagian convert type
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

string createStringFromDiagnosa(Diagnosa &diagnosaData)
{
    ostringstream oss;
    oss << diagnosaData->id << "#"
        << diagnosaData->hasilDiagnosa << "#"
        << diagnosaData->waktuDiagnosa;

    // Tambahkan informasi mengenai treatment jika ada
    Treatment currentTreatment = diagnosaData->treatmentDiagnosa;
    while (currentTreatment != NULL)
    {
        oss << "#" << currentTreatment->id << "#"
            << currentTreatment->namaTreatment << "#"
            << currentTreatment->biaya;

        currentTreatment = currentTreatment->next;
    }

    return oss.str();
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

class DatabaseManager
{
private:
    sqlite3 *db;
    // Singleton class
    DatabaseManager(const char *dbName)
    {
        int rc = sqlite3_open(dbName, &db);

        if (rc != SQLITE_OK)
        {
            std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
            exit(1);
        }

        // Create tables if not exists
        createTables();
    }

    ~DatabaseManager()
    {
        sqlite3_close(db);
    }

    static DatabaseManager *instance;

public:
    static DatabaseManager &getInstance()
    {
        // This ensures that the instance is created only once
        static DatabaseManager instance("database.db");
        return instance;
    }

    void createTables()
    {
        int rc;
        const char *sqlPasien = "CREATE TABLE IF NOT EXISTS pasien ("
                                "nomor INTEGER PRIMARY KEY AUTOINCREMENT,"
                                "nama TEXT NOT NULL,"
                                "alamat TEXT NOT NULL,"
                                "jenisKelamin TEXT NOT NULL,"
                                "tempatLahir TEXT NOT NULL,"
                                "tanggalLahir TEXT NOT NULL,"
                                "kontak TEXT NOT NULL);";

        const char *sqlDiagnosa = "CREATE TABLE IF NOT EXISTS diagnosa ("
                                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                  "hasilDiagnosa TEXT NOT NULL,"
                                  "waktuDiagnosa TEXT NOT NULL,"
                                  "pasien_nomor INTEGER,"
                                  "FOREIGN KEY (pasien_nomor) REFERENCES pasien(nomor));";

        const char *sqlTreatment = "CREATE TABLE IF NOT EXISTS treatment ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "namaTreatment TEXT NOT NULL,"
                                   "biaya INTEGER NOT NULL,"
                                   "diagnosa_id INTEGER,"
                                   "FOREIGN KEY (diagnosa_id) REFERENCES diagnosa(id));";

        const char *sqlRiwayatTransaksi = "CREATE TABLE IF NOT EXISTS riwayattransaksi ("
                                          "nomorTransaksi INTEGER PRIMARY KEY AUTOINCREMENT,"
                                          "idPasien INTEGER NOT NULL,"
                                          "idDiagnosa INTEGER NOT NULL,"
                                          "waktuTransaksi TEXT NOT NULL,"
                                          "namaPasien TEXT NOT NULL,"
                                          "total INTEGER NOT NULL,"
                                          "jumlahDibayarkan INTEGER NOT NULL)";
        const char *userdata =
            "CREATE TABLE IF NOT EXISTS userdata ("
            "    no INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    username TEXT NOT NULL,"
            "    password TEXT NOT NULL,"
            "    usertype INTEGER NOT NULL"
            ");";

        const char *dokter =
            "CREATE TABLE IF NOT EXISTS dokter ("
            "    no INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    username TEXT NOT NULL,"
            "    nama TEXT NOT NULL,"
            "    poliklinik INTEGER NOT NULL,"
            "    password TEXT NOT NULL"
            ");";

        rc = sqlite3_exec(db, sqlPasien, 0, 0, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot create pasien table: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        rc = sqlite3_exec(db, sqlDiagnosa, 0, 0, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot create diagnosa table: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        rc = sqlite3_exec(db, sqlTreatment, 0, 0, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot create treatment table: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        rc = sqlite3_exec(db, sqlRiwayatTransaksi, 0, 0, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot create Riwayat Transaksi table: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        rc = sqlite3_exec(db, dokter, 0, 0, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot create Dokter table: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        rc = sqlite3_exec(db, userdata, 0, 0, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot create User Data table: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }
    }
    // Bagian Pasien dan RiwayatTransaksi
    bool readPasienArray(pasienArray &pasienList)
    {
        pasienArray result;
        int rc;

        const char *query = "SELECT * FROM pasien";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK)
        {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Pasien pasien;

            pasien.nomor = sqlite3_column_int(stmt, 0);
            pasien.dataPribadi.nama = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            pasien.dataPribadi.alamat = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            pasien.dataPribadi.jenisKelamin = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
            pasien.dataPribadi.tempatLahir = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
            pasien.dataPribadi.tanggalLahir = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
            pasien.dataPribadi.kontak = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

            // TODO: Retrieve and connect other data (diagnosaPasien)
            const char *sqlDiagnosa = "SELECT * FROM diagnosa WHERE pasien_nomor = ?;";
            sqlite3_stmt *stmtDiagnosa;

            rc = sqlite3_prepare_v2(db, sqlDiagnosa, -1, &stmtDiagnosa, 0);

            if (rc != SQLITE_OK)
            {
                cerr << "Cannot prepare statement for diagnosaPasien: " << sqlite3_errmsg(db) << endl;

                return false;
            }

            sqlite3_bind_int(stmtDiagnosa, 1, pasien.nomor);
            pasien.diagnosaPasien = NULL;
            while ((rc = sqlite3_step(stmtDiagnosa)) == SQLITE_ROW)
            {
                Diagnosa pDiagnosa = new diagnosa;
                pDiagnosa->id = sqlite3_column_int(stmtDiagnosa, 3);
                pDiagnosa->hasilDiagnosa = reinterpret_cast<const char *>(sqlite3_column_text(stmtDiagnosa, 1));
                pDiagnosa->waktuDiagnosa = reinterpret_cast<const char *>(sqlite3_column_text(stmtDiagnosa, 2));
                pDiagnosa->next = NULL;

                // TODO: Retrieve and connect treatment data for this diagnosis
                const char *sqlTreatment = "SELECT * FROM treatment WHERE diagnosa_id = ?;";
                sqlite3_stmt *stmtTreatment;

                rc = sqlite3_prepare_v2(db, sqlTreatment, -1, &stmtTreatment, 0);

                if (rc != SQLITE_OK)
                {
                    cerr << "Cannot prepare statement for treatment: " << sqlite3_errmsg(db) << endl;

                    return false;
                }

                sqlite3_bind_int(stmtTreatment, 1, pDiagnosa->id);
                pDiagnosa->treatmentDiagnosa = NULL;
                while ((rc = sqlite3_step(stmtTreatment)) == SQLITE_ROW)
                {
                    Treatment pTreatment = new treatment;
                    pTreatment->id = sqlite3_column_int(stmtTreatment, 3);
                    pTreatment->namaTreatment = reinterpret_cast<const char *>(sqlite3_column_text(stmtTreatment, 1));
                    pTreatment->biaya = sqlite3_column_int(stmtTreatment, 2);
                    pTreatment->next = NULL;
                    if (pTreatment->id == pDiagnosa->id)
                    {
                        insertLastTreatment(pDiagnosa->treatmentDiagnosa, pTreatment);
                    }
                    delete pTreatment;
                }
                sqlite3_finalize(stmtTreatment);
                if (pasien.nomor == pDiagnosa->id)
                {
                    insertLastDiagnosa(pasien.diagnosaPasien, pDiagnosa);
                }
                delete pDiagnosa;
            }
            sqlite3_finalize(stmtDiagnosa);

            result.push_back(pasien);
        }

        sqlite3_finalize(stmt);

        return true;
    }

    bool ReadPasienByID(int nomorPasien, Pasien &pPasien)
    {
        int rc;
        const char *query = "SELECT * FROM pasien WHERE nomor = ?";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK)
        {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_int(stmt, 1, nomorPasien);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            pPasien.nomor = sqlite3_column_int(stmt, 0);
            pPasien.dataPribadi.nama = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            pPasien.dataPribadi.alamat = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            pPasien.dataPribadi.jenisKelamin = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
            pPasien.dataPribadi.tempatLahir = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
            pPasien.dataPribadi.tanggalLahir = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
            pPasien.dataPribadi.kontak = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

            // TODO: Retrieve and connect other data (diagnosaPasien)
            const char *sqlDiagnosa = "SELECT * FROM diagnosa WHERE pasien_nomor = ?;";
            sqlite3_stmt *stmtDiagnosa;

            rc = sqlite3_prepare_v2(db, sqlDiagnosa, -1, &stmtDiagnosa, 0);

            if (rc != SQLITE_OK)
            {
                cerr << "Cannot prepare statement for diagnosaPasien: " << sqlite3_errmsg(db) << endl;
                return false;
            }

            sqlite3_bind_int(stmtDiagnosa, 1, nomorPasien);

            while ((rc = sqlite3_step(stmtDiagnosa)) == SQLITE_ROW)
            {
                Diagnosa pDiagnosa = new diagnosa;
                pDiagnosa->id = sqlite3_column_int(stmtDiagnosa, 0);
                pDiagnosa->hasilDiagnosa = reinterpret_cast<const char *>(sqlite3_column_text(stmtDiagnosa, 1));
                pDiagnosa->waktuDiagnosa = reinterpret_cast<const char *>(sqlite3_column_text(stmtDiagnosa, 2));
                pDiagnosa->next = NULL;

                // TODO: Retrieve and connect treatment data for this diagnosis
                const char *sqlTreatment = "SELECT * FROM treatment WHERE diagnosa_id = ?;";
                sqlite3_stmt *stmtTreatment;

                rc = sqlite3_prepare_v2(db, sqlTreatment, -1, &stmtTreatment, 0);

                if (rc != SQLITE_OK)
                {
                    cerr << "Cannot prepare statement for treatment: " << sqlite3_errmsg(db) << endl;
                    return false;
                }

                sqlite3_bind_int(stmtTreatment, 1, pDiagnosa->id);

                while ((rc = sqlite3_step(stmtTreatment)) == SQLITE_ROW)
                {
                    Treatment pTreatment = new treatment;
                    pTreatment->id = sqlite3_column_int(stmtTreatment, 0);
                    pTreatment->namaTreatment = reinterpret_cast<const char *>(sqlite3_column_text(stmtTreatment, 1));
                    pTreatment->biaya = sqlite3_column_int(stmtTreatment, 2);
                    pTreatment->next = NULL;

                    // TODO: Connect the treatment to the diagnosis
                    if (pTreatment->id == pDiagnosa->id)
                    {
                        insertLastTreatment(pDiagnosa->treatmentDiagnosa, pTreatment);
                    }
                    else
                    {
                        delete pTreatment;
                    }
                }
                sqlite3_finalize(stmtTreatment);

                // TODO: Connect the diagnosis to the patient
                insertLastDiagnosa(pPasien.diagnosaPasien, pDiagnosa);
            }

            sqlite3_finalize(stmtDiagnosa);
            sqlite3_finalize(stmt);
            return true;
        }
        else
        {
            cerr << "No data found for Pasien with nomor: " << nomorPasien << endl;
            sqlite3_finalize(stmt);
            return false;
        }
    }

    bool readRiwayatTransaksiArray(riwayatTransaksiArray &riwayatTransaksiList)
    {
        const char *sqlRiwayatTransaksi = "SELECT * FROM riwayattransaksi;";
        sqlite3_stmt *stmtRiwayatTransaksi;

        if (sqlite3_prepare_v2(db, sqlRiwayatTransaksi, -1, &stmtRiwayatTransaksi, NULL) != SQLITE_OK)
        {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        while (sqlite3_step(stmtRiwayatTransaksi) == SQLITE_ROW)
        {
            riwayatTransaksi rt;
            rt.nomorTransaksi = sqlite3_column_int(stmtRiwayatTransaksi, 0);
            rt.idPasien = sqlite3_column_int(stmtRiwayatTransaksi, 1);
            rt.idDiagnosa = sqlite3_column_int(stmtRiwayatTransaksi, 2);
            rt.waktuTransaksi = reinterpret_cast<const char *>(sqlite3_column_text(stmtRiwayatTransaksi, 3));
            rt.namaPasien = reinterpret_cast<const char *>(sqlite3_column_text(stmtRiwayatTransaksi, 4));
            rt.total = sqlite3_column_int(stmtRiwayatTransaksi, 5);
            rt.jumlahDibayarkan = sqlite3_column_int(stmtRiwayatTransaksi, 6);

            rt.treatmentPasien = NULL;

            const char *sqlTreatment = "SELECT * FROM treatment WHERE diagnosa_id = ?;";
            sqlite3_stmt *stmtTreatment;

            if (sqlite3_prepare_v2(db, sqlTreatment, -1, &stmtTreatment, NULL) != SQLITE_OK)
            {
                cerr << "Failed to prepare statement for treatment: " << sqlite3_errmsg(db) << endl;
                return false;
            }

            sqlite3_bind_int(stmtTreatment, 1, rt.idDiagnosa);

            while (sqlite3_step(stmtTreatment) == SQLITE_ROW)
            {
                Treatment pTreatment = new treatment;
                pTreatment->id = sqlite3_column_int(stmtTreatment, 0);
                pTreatment->namaTreatment = reinterpret_cast<const char *>(sqlite3_column_text(stmtTreatment, 1));
                pTreatment->biaya = sqlite3_column_int(stmtTreatment, 2);
                pTreatment->next = NULL;
                insertLastTreatment(rt.treatmentPasien, pTreatment);
            }
            sqlite3_finalize(stmtTreatment);

            // Add the transaction to the list
            riwayatTransaksiList.push_back(rt);
        }

        sqlite3_finalize(stmtRiwayatTransaksi);
        return true;
    }

    bool ReadRiwayatTransaksiByID(int nomorTransaksi, riwayatTransaksi &rt)
    {
        const char *sqlRiwayatTransaksi = "SELECT * FROM riwayattransaksi WHERE nomorTransaksi = ?;";
        sqlite3_stmt *stmtRiwayatTransaksi;

        if (sqlite3_prepare_v2(db, sqlRiwayatTransaksi, -1, &stmtRiwayatTransaksi, NULL) != SQLITE_OK)
        {
            cerr << "Failed to prepare statement for riwayattransaksi: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_int(stmtRiwayatTransaksi, 1, nomorTransaksi);

        if (sqlite3_step(stmtRiwayatTransaksi) == SQLITE_ROW)
        {
            rt.nomorTransaksi = sqlite3_column_int(stmtRiwayatTransaksi, 0);
            rt.idPasien = sqlite3_column_int(stmtRiwayatTransaksi, 1);
            rt.idDiagnosa = sqlite3_column_int(stmtRiwayatTransaksi, 2);
            rt.waktuTransaksi = reinterpret_cast<const char *>(sqlite3_column_text(stmtRiwayatTransaksi, 3));
            rt.namaPasien = reinterpret_cast<const char *>(sqlite3_column_text(stmtRiwayatTransaksi, 4));
            rt.total = sqlite3_column_int(stmtRiwayatTransaksi, 5);
            rt.jumlahDibayarkan = sqlite3_column_int(stmtRiwayatTransaksi, 6);

            rt.treatmentPasien = NULL;

            const char *sqlTreatment = "SELECT * FROM treatment WHERE diagnosa_id = ?;";
            sqlite3_stmt *stmtTreatment;

            if (sqlite3_prepare_v2(db, sqlTreatment, -1, &stmtTreatment, NULL) != SQLITE_OK)
            {
                cerr << "Failed to prepare statement for treatment: " << sqlite3_errmsg(db) << endl;
                return false;
            }

            sqlite3_bind_int(stmtTreatment, 1, rt.idDiagnosa);

            while (sqlite3_step(stmtTreatment) == SQLITE_ROW)
            {
                Treatment pTreatment = new treatment;
                pTreatment->id = sqlite3_column_int(stmtTreatment, 0);
                pTreatment->namaTreatment = reinterpret_cast<const char *>(sqlite3_column_text(stmtTreatment, 1));
                pTreatment->biaya = sqlite3_column_int(stmtTreatment, 2);
                pTreatment->next = NULL;
                insertLastTreatment(rt.treatmentPasien, pTreatment);
            }
            sqlite3_finalize(stmtTreatment);

            sqlite3_finalize(stmtRiwayatTransaksi);
            return true;
        }
        else
        {
            cerr << "Riwayat Transaksi not found with nomorTransaksi: " << nomorTransaksi << endl;
            sqlite3_finalize(stmtRiwayatTransaksi);
            return false;
        }
    }

    bool insertPasien(Pasien pPasien, int lastInsertID)
    {
        const char *sqlInsertPasien = "INSERT INTO pasien (nama, alamat, jenisKelamin, tempatLahir, tanggalLahir, kontak) VALUES (?, ?, ?, ?, ?, ?);";
        sqlite3_stmt *stmtInsertPasien;

        if (sqlite3_prepare_v2(db, sqlInsertPasien, -1, &stmtInsertPasien, NULL) != SQLITE_OK)
        {
            cerr << "Failed to prepare statement for inserting pasien: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_text(stmtInsertPasien, 1, pPasien.dataPribadi.nama.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtInsertPasien, 2, pPasien.dataPribadi.alamat.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtInsertPasien, 3, pPasien.dataPribadi.jenisKelamin.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtInsertPasien, 4, pPasien.dataPribadi.tempatLahir.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtInsertPasien, 5, pPasien.dataPribadi.tanggalLahir.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtInsertPasien, 6, pPasien.dataPribadi.kontak.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmtInsertPasien) != SQLITE_DONE)
        {
            cerr << "Error inserting pasien: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmtInsertPasien);
            return false;
        }

        // Get the last inserted ID
        lastInsertID = sqlite3_last_insert_rowid(db);

        // Insert diagnosa data
        Diagnosa pDiagnosa = pPasien.diagnosaPasien;
        while (pDiagnosa != NULL)
        {
            const char *sqlInsertDiagnosa = "INSERT INTO diagnosa (pasien_nomor, hasilDiagnosa, waktuDiagnosa) VALUES (?, ?, ?);";
            sqlite3_stmt *stmtInsertDiagnosa;

            if (sqlite3_prepare_v2(db, sqlInsertDiagnosa, -1, &stmtInsertDiagnosa, NULL) != SQLITE_OK)
            {
                cerr << "Failed to prepare statement for inserting diagnosa: " << sqlite3_errmsg(db) << endl;
                return false;
            }

            sqlite3_bind_int(stmtInsertDiagnosa, 1, lastInsertID);
            sqlite3_bind_text(stmtInsertDiagnosa, 2, pDiagnosa->hasilDiagnosa.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmtInsertDiagnosa, 3, pDiagnosa->waktuDiagnosa.c_str(), -1, SQLITE_STATIC);

            if (sqlite3_step(stmtInsertDiagnosa) != SQLITE_DONE)
            {
                cerr << "Error inserting diagnosa: " << sqlite3_errmsg(db) << endl;
                sqlite3_finalize(stmtInsertDiagnosa);
                return false;
            }

            // Get the last inserted ID for diagnosa
            int lastInsertDiagnosaID = sqlite3_last_insert_rowid(db);

            // Insert treatment data
            Treatment pTreatment = pDiagnosa->treatmentDiagnosa;
            while (pTreatment != NULL)
            {
                const char *sqlInsertTreatment = "INSERT INTO treatment (diagnosa_id, namaTreatment, biaya) VALUES (?, ?, ?);";
                sqlite3_stmt *stmtInsertTreatment;

                if (sqlite3_prepare_v2(db, sqlInsertTreatment, -1, &stmtInsertTreatment, NULL) != SQLITE_OK)
                {
                    cerr << "Failed to prepare statement for inserting treatment: " << sqlite3_errmsg(db) << endl;
                    return false;
                }

                sqlite3_bind_int(stmtInsertTreatment, 1, lastInsertDiagnosaID);
                sqlite3_bind_text(stmtInsertTreatment, 2, pTreatment->namaTreatment.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_int(stmtInsertTreatment, 3, pTreatment->biaya);

                if (sqlite3_step(stmtInsertTreatment) != SQLITE_DONE)
                {
                    cerr << "Error inserting treatment: " << sqlite3_errmsg(db) << endl;
                    sqlite3_finalize(stmtInsertTreatment);
                    return false;
                }

                sqlite3_finalize(stmtInsertTreatment);

                pTreatment = pTreatment->next;
            }

            sqlite3_finalize(stmtInsertDiagnosa);
            pDiagnosa = pDiagnosa->next;
        }

        sqlite3_finalize(stmtInsertPasien);
        return true;
    }

    bool insertRiwayatTransaksi(riwayatTransaksi rt)
    {
        const char *sqlInsertRiwayatTransaksi = "INSERT INTO riwayattransaksi (idPasien, idDiagnosa, waktuTransaksi, namaPasien, total, jumlahDibayarkan) VALUES (?, ?, ?, ?, ?, ?);";
        sqlite3_stmt *stmtInsertRiwayatTransaksi;

        if (sqlite3_prepare_v2(db, sqlInsertRiwayatTransaksi, -1, &stmtInsertRiwayatTransaksi, NULL) != SQLITE_OK)
        {
            cerr << "Failed to prepare statement for inserting riwayattransaksi: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_int(stmtInsertRiwayatTransaksi, 1, rt.idPasien);
        sqlite3_bind_int(stmtInsertRiwayatTransaksi, 2, rt.idDiagnosa);
        sqlite3_bind_text(stmtInsertRiwayatTransaksi, 3, rt.waktuTransaksi.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtInsertRiwayatTransaksi, 4, rt.namaPasien.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmtInsertRiwayatTransaksi, 5, rt.total);
        sqlite3_bind_int(stmtInsertRiwayatTransaksi, 6, rt.jumlahDibayarkan);

        if (sqlite3_step(stmtInsertRiwayatTransaksi) != SQLITE_DONE)
        {
            cerr << "Error inserting riwayattransaksi: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmtInsertRiwayatTransaksi);
            return false;
        }

        // Get the last inserted ID for riwayattransaksi
        int lastInsertTransaksiID = sqlite3_last_insert_rowid(db);

        // Insert treatment data
        Treatment pTreatment = rt.treatmentPasien;
        while (pTreatment != NULL)
        {
            const char *sqlInsertTreatment = "INSERT INTO treatment (diagnosa_id, namaTreatment, biaya) VALUES (?, ?, ?);";
            sqlite3_stmt *stmtInsertTreatment;

            if (sqlite3_prepare_v2(db, sqlInsertTreatment, -1, &stmtInsertTreatment, NULL) != SQLITE_OK)
            {
                cerr << "Failed to prepare statement for inserting treatment: " << sqlite3_errmsg(db) << endl;
                return false;
            }

            sqlite3_bind_int(stmtInsertTreatment, 1, lastInsertTransaksiID);
            sqlite3_bind_text(stmtInsertTreatment, 2, pTreatment->namaTreatment.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmtInsertTreatment, 3, pTreatment->biaya);

            if (sqlite3_step(stmtInsertTreatment) != SQLITE_DONE)
            {
                cerr << "Error inserting treatment: " << sqlite3_errmsg(db) << endl;
                sqlite3_finalize(stmtInsertTreatment);
                return false;
            }

            sqlite3_finalize(stmtInsertTreatment);
            pTreatment = pTreatment->next;
        }

        sqlite3_finalize(stmtInsertRiwayatTransaksi);
        return true;
    }

    bool updatePasienByID(int nomorPasien, Pasien pPasien)
    {
        const char *sqlUpdatePasien = "UPDATE pasien SET nama = ?, alamat = ?, jenisKelamin = ?, tempatLahir = ?, tanggalLahir = ?, kontak = ? WHERE nomor = ?;";
        sqlite3_stmt *stmtUpdatePasien;

        if (sqlite3_prepare_v2(db, sqlUpdatePasien, -1, &stmtUpdatePasien, NULL) != SQLITE_OK)
        {
            cerr << "Failed to prepare statement for updating pasien: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_text(stmtUpdatePasien, 1, pPasien.dataPribadi.nama.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtUpdatePasien, 2, pPasien.dataPribadi.alamat.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtUpdatePasien, 3, pPasien.dataPribadi.jenisKelamin.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtUpdatePasien, 4, pPasien.dataPribadi.tempatLahir.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtUpdatePasien, 5, pPasien.dataPribadi.tanggalLahir.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtUpdatePasien, 6, pPasien.dataPribadi.kontak.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmtUpdatePasien, 7, nomorPasien);

        if (sqlite3_step(stmtUpdatePasien) != SQLITE_DONE)
        {
            cerr << "Error updating pasien: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmtUpdatePasien);
            return false;
        }

        // Update diagnosa data
        Diagnosa pDiagnosa = pPasien.diagnosaPasien;
        while (pDiagnosa != NULL)
        {
            const char *sqlUpdateDiagnosa = "UPDATE diagnosa SET hasilDiagnosa = ?, waktuDiagnosa = ? WHERE id = ?;";
            sqlite3_stmt *stmtUpdateDiagnosa;

            if (sqlite3_prepare_v2(db, sqlUpdateDiagnosa, -1, &stmtUpdateDiagnosa, NULL) != SQLITE_OK)
            {
                cerr << "Failed to prepare statement for updating diagnosa: " << sqlite3_errmsg(db) << endl;
                return false;
            }

            sqlite3_bind_text(stmtUpdateDiagnosa, 1, pDiagnosa->hasilDiagnosa.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmtUpdateDiagnosa, 2, pDiagnosa->waktuDiagnosa.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmtUpdateDiagnosa, 3, pDiagnosa->id);

            if (sqlite3_step(stmtUpdateDiagnosa) != SQLITE_DONE)
            {
                cerr << "Error updating diagnosa: " << sqlite3_errmsg(db) << endl;
                sqlite3_finalize(stmtUpdateDiagnosa);
                return false;
            }

            // Update treatment data
            Treatment pTreatment = pDiagnosa->treatmentDiagnosa;
            while (pTreatment != NULL)
            {
                const char *sqlUpdateTreatment = "UPDATE treatment SET namaTreatment = ?, biaya = ? WHERE id = ?;";
                sqlite3_stmt *stmtUpdateTreatment;

                if (sqlite3_prepare_v2(db, sqlUpdateTreatment, -1, &stmtUpdateTreatment, NULL) != SQLITE_OK)
                {
                    cerr << "Failed to prepare statement for updating treatment: " << sqlite3_errmsg(db) << endl;
                    return false;
                }

                sqlite3_bind_text(stmtUpdateTreatment, 1, pTreatment->namaTreatment.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_int(stmtUpdateTreatment, 2, pTreatment->biaya);
                sqlite3_bind_int(stmtUpdateTreatment, 3, pTreatment->id);

                if (sqlite3_step(stmtUpdateTreatment) != SQLITE_DONE)
                {
                    cerr << "Error updating treatment: " << sqlite3_errmsg(db) << endl;
                    sqlite3_finalize(stmtUpdateTreatment);
                    return false;
                }

                sqlite3_finalize(stmtUpdateTreatment);

                pTreatment = pTreatment->next;
            }

            sqlite3_finalize(stmtUpdateDiagnosa);
            pDiagnosa = pDiagnosa->next;
        }

        sqlite3_finalize(stmtUpdatePasien);
        return true;
    }

    bool updateRiwayatTransaksiByID(int nomorTransaksi, riwayatTransaksi rt)
    {
        const char *sqlUpdateRiwayatTransaksi = "UPDATE riwayattransaksi SET idPasien = ?, idDiagnosa = ?, waktuTransaksi = ?, namaPasien = ?, total = ?, jumlahDibayarkan = ? WHERE nomorTransaksi = ?;";
        sqlite3_stmt *stmtUpdateRiwayatTransaksi;

        if (sqlite3_prepare_v2(db, sqlUpdateRiwayatTransaksi, -1, &stmtUpdateRiwayatTransaksi, NULL) != SQLITE_OK)
        {
            cerr << "Failed to prepare statement for updating riwayattransaksi: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_int(stmtUpdateRiwayatTransaksi, 1, rt.idPasien);
        sqlite3_bind_int(stmtUpdateRiwayatTransaksi, 2, rt.idDiagnosa);
        sqlite3_bind_text(stmtUpdateRiwayatTransaksi, 3, rt.waktuTransaksi.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtUpdateRiwayatTransaksi, 4, rt.namaPasien.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmtUpdateRiwayatTransaksi, 5, rt.total);
        sqlite3_bind_int(stmtUpdateRiwayatTransaksi, 6, rt.jumlahDibayarkan);
        sqlite3_bind_int(stmtUpdateRiwayatTransaksi, 7, nomorTransaksi);

        if (sqlite3_step(stmtUpdateRiwayatTransaksi) != SQLITE_DONE)
        {
            cerr << "Error updating riwayattransaksi: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmtUpdateRiwayatTransaksi);
            return false;
        }

        // Update treatment data
        Treatment pTreatment = rt.treatmentPasien;
        while (pTreatment != NULL)
        {
            const char *sqlUpdateTreatment = "UPDATE treatment SET namaTreatment = ?, biaya = ? WHERE id = ?;";
            sqlite3_stmt *stmtUpdateTreatment;

            if (sqlite3_prepare_v2(db, sqlUpdateTreatment, -1, &stmtUpdateTreatment, NULL) != SQLITE_OK)
            {
                cerr << "Failed to prepare statement for updating treatment: " << sqlite3_errmsg(db) << endl;
                return false;
            }

            sqlite3_bind_text(stmtUpdateTreatment, 1, pTreatment->namaTreatment.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmtUpdateTreatment, 2, pTreatment->biaya);
            sqlite3_bind_int(stmtUpdateTreatment, 3, pTreatment->id);

            if (sqlite3_step(stmtUpdateTreatment) != SQLITE_DONE)
            {
                cerr << "Error updating treatment: " << sqlite3_errmsg(db) << endl;
                sqlite3_finalize(stmtUpdateTreatment);
                return false;
            }

            sqlite3_finalize(stmtUpdateTreatment);

            pTreatment = pTreatment->next;
        }

        sqlite3_finalize(stmtUpdateRiwayatTransaksi);
        return true;
    }

    bool deletePasien(int nomorPasien)
    {
        const char *sqlDeletePasien = "DELETE FROM pasien WHERE nomor = ?;";
        sqlite3_stmt *stmtDeletePasien;

        if (sqlite3_prepare_v2(db, sqlDeletePasien, -1, &stmtDeletePasien, NULL) != SQLITE_OK)
        {
            cerr << "Failed to prepare statement for deleting pasien: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_int(stmtDeletePasien, 1, nomorPasien);

        if (sqlite3_step(stmtDeletePasien) != SQLITE_DONE)
        {
            cerr << "Error deleting pasien: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmtDeletePasien);
            return false;
        }

        // Delete diagnosa data
        const char *sqlDeleteDiagnosa = "DELETE FROM diagnosa WHERE pasien_nomor = ?;";
        sqlite3_stmt *stmtDeleteDiagnosa;

        if (sqlite3_prepare_v2(db, sqlDeleteDiagnosa, -1, &stmtDeleteDiagnosa, NULL) != SQLITE_OK)
        {
            cerr << "Failed to prepare statement for deleting diagnosa: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_int(stmtDeleteDiagnosa, 1, nomorPasien);

        if (sqlite3_step(stmtDeleteDiagnosa) != SQLITE_DONE)
        {
            cerr << "Error deleting diagnosa: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmtDeleteDiagnosa);
            return false;
        }

        // Delete treatment data
        const char *sqlDeleteTreatment = "DELETE FROM treatment WHERE diagnosa_id IN (SELECT id FROM diagnosa WHERE pasien_nomor = ?);";
        sqlite3_stmt *stmtDeleteTreatment;

        if (sqlite3_prepare_v2(db, sqlDeleteTreatment, -1, &stmtDeleteTreatment, NULL) != SQLITE_OK)
        {
            cerr << "Failed to prepare statement for deleting treatment: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_int(stmtDeleteTreatment, 1, nomorPasien);

        if (sqlite3_step(stmtDeleteTreatment) != SQLITE_DONE)
        {
            cerr << "Error deleting treatment: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmtDeleteTreatment);
            return false;
        }

        sqlite3_finalize(stmtDeletePasien);
        sqlite3_finalize(stmtDeleteDiagnosa);
        sqlite3_finalize(stmtDeleteTreatment);

        return true;
    }

    bool deleteRiwayatTransaksi(int nomorTransaksi)
    {
        const char *sqlDeleteRiwayatTransaksi = "DELETE FROM riwayattransaksi WHERE nomorTransaksi = ?;";
        sqlite3_stmt *stmtDeleteRiwayatTransaksi;

        if (sqlite3_prepare_v2(db, sqlDeleteRiwayatTransaksi, -1, &stmtDeleteRiwayatTransaksi, NULL) != SQLITE_OK)
        {
            cerr << "Failed to prepare statement for deleting riwayattransaksi: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_int(stmtDeleteRiwayatTransaksi, 1, nomorTransaksi);

        if (sqlite3_step(stmtDeleteRiwayatTransaksi) != SQLITE_DONE)
        {
            cerr << "Error deleting riwayattransaksi: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmtDeleteRiwayatTransaksi);
            return false;
        }

        // Delete treatment data
        const char *sqlDeleteTreatment = "DELETE FROM treatment WHERE diagnosa_id = ?;";
        sqlite3_stmt *stmtDeleteTreatment;

        if (sqlite3_prepare_v2(db, sqlDeleteTreatment, -1, &stmtDeleteTreatment, NULL) != SQLITE_OK)
        {
            cerr << "Failed to prepare statement for deleting treatment: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_int(stmtDeleteTreatment, 1, nomorTransaksi);

        if (sqlite3_step(stmtDeleteTreatment) != SQLITE_DONE)
        {
            cerr << "Error deleting treatment: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmtDeleteTreatment);
            return false;
        }

        sqlite3_finalize(stmtDeleteRiwayatTransaksi);
        sqlite3_finalize(stmtDeleteTreatment);

        return true;
    }
    // Bagian UserData
    bool verifyUserdata(string &username, string &password, UserType &userType)
    {
        int rc;
        const char *sql = "SELECT * FROM userdata WHERE username = ? AND password = ?;";
        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;

            return false;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW)
        {
            userType = static_cast<UserType>(sqlite3_column_int(stmt, 3));
            sqlite3_finalize(stmt);

            return true;
        }

        sqlite3_finalize(stmt);

        return false;
    }

    bool updateUserdataByUsername(string username, UserData userdata)
    {
        int rc;
        const char *sql = "UPDATE userdata SET password = ?, usertype = ? WHERE username = ?;";
        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, userdata.password.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, static_cast<int>(userdata.usertype));
        sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            cerr << "Execution failed: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        return true;
    }

    bool updateDokterByUsername(string username, Dokter dataDokter)
    {
        int rc;
        const char *sql = "UPDATE dokter SET password = ?, nama = ?, poliklinik = ? WHERE username = ?;";
        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, dataDokter.password.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, dataDokter.nama.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, static_cast<int>(dataDokter.poliklinik));
        sqlite3_bind_text(stmt, 4, username.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            cerr << "Execution failed: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        return true;
    }

    void readUsername(userdataArray &dataAllUsername)
    {
        const char *sql = "SELECT * FROM userdata;";
        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            UserData userData;
            userData.username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            userData.password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            userData.usertype = static_cast<UserType>(sqlite3_column_int(stmt, 3));

            dataAllUsername.push_back(userData);
        }

        sqlite3_finalize(stmt);
    }

    bool addUserdata(UserData userdata)
    {
        const char *sqlUserdata = "INSERT INTO userdata (username, password, usertype) VALUES (?, ?, ?);";
        sqlite3_stmt *stmtUserdata;
        int rc = sqlite3_prepare_v2(db, sqlUserdata, -1, &stmtUserdata, nullptr);

        if (rc != SQLITE_OK)
        {
            std::cerr << "Cannot prepare statement for inserting userdata: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        if (sqlite3_bind_text(stmtUserdata, 1, userdata.username.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
            sqlite3_bind_text(stmtUserdata, 2, userdata.password.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
            sqlite3_bind_int(stmtUserdata, 3, static_cast<int>(userdata.usertype)) != SQLITE_OK)
        {
            std::cerr << "Error binding values: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmtUserdata);
            return false;
        }
        rc = sqlite3_step(stmtUserdata);

        if (rc != SQLITE_DONE)
        {
            std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmtUserdata);
            return false;
        }

        sqlite3_finalize(stmtUserdata);
        return true;
    }

    bool checkUserIfExists(const std::string &username, UserType &usertype)
    {
        const char *sql = "SELECT usertype FROM userdata WHERE username = ?;";
        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        bool result = false;
        if ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            usertype = static_cast<UserType>(sqlite3_column_int(stmt, 3));
            result = true;
        }

        sqlite3_finalize(stmt);
        return result;
    }
    // Bagian Dokter

    bool deleteUserdataByUsername(string username)
    {
        int rc;
        const char *sql = "DELETE FROM userdata WHERE username = ?;";
        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;

            return false;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            cerr << "Execution failed: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);

            return false;
        }

        sqlite3_finalize(stmt);

        return true;
    }

    bool deleteDokterByUsername(string username)
    {
        int rc;
        const char *sql = "DELETE FROM dokter WHERE username = ?;";
        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;

            return false;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            cerr << "Execution failed: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);

            return false;
        }

        sqlite3_finalize(stmt);

        return true;
    }

    void readDokter(dokterArray &dataAllDokter)
    {
        const char *sql = "SELECT * FROM dokter;";
        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Dokter dokter;
            dokter.username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            dokter.nama = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            dokter.poliklinik = static_cast<Poliklinik>(sqlite3_column_int(stmt, 3));
            dokter.password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));

            dataAllDokter.push_back(dokter);
        }

        sqlite3_finalize(stmt);
    }

    bool readDokterByUsername(const std::string &username, Dokter &dataDokter)
    {
        const char *sql = "SELECT * FROM dokter WHERE username = ?;";
        sqlite3_stmt *stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            std::cerr << "Cannot prepare statement for reading dokter: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if (rc == SQLITE_ROW)
        {
            // Retrieve data from the result set
            dataDokter.username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            dataDokter.nama = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            dataDokter.poliklinik = static_cast<Poliklinik>(sqlite3_column_int(stmt, 3));
            dataDokter.password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));

            sqlite3_finalize(stmt);
            return true;
        }

        sqlite3_finalize(stmt);
        return false;
    }

    bool addDokter(Dokter &dataDokter)
    {
        const char *sqlDokter = "INSERT INTO dokter (username, nama, poliklinik,password) VALUES (?, ?, ?, ?);";
        sqlite3_stmt *stmtDokter;
        int rc = sqlite3_prepare_v2(db, sqlDokter, -1, &stmtDokter, 0);

        if (rc != SQLITE_OK)
        {
            std::cerr << "Cannot prepare statement for inserting dokter: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        sqlite3_bind_text(stmtDokter, 2, dataDokter.nama.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmtDokter, 3, static_cast<int>(dataDokter.poliklinik));
        sqlite3_bind_text(stmtDokter, 1, dataDokter.username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtDokter, 4, dataDokter.password.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmtDokter);

        if (rc != SQLITE_DONE)
        {
            std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmtDokter);
            return false;
        }

        sqlite3_finalize(stmtDokter);
        return true;
    }

    bool isDokterExistByName(string &nama)
    {
        const char *sql = "SELECT COUNT(*) FROM dokter WHERE nama = ?;";
        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, nama.c_str(), -1, SQLITE_STATIC);

        bool result = false;
        if ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            int count = sqlite3_column_int(stmt, 1);
            result = (count > 0);
        }

        sqlite3_finalize(stmt);
        return result;
    }
};

// void displayOutputPemeriksaan(Pasien first)
// {
//     Pasien pBantu = first;
//     cout << "Data Pemeriksaan Awal" << endl;
//     cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;
//     cout << setw(15) << left << "|Berat Badan"
//          << setw(15) << "|Tinggi Badan"
//          << setw(15) << "|Suhu Badan"
//          << setw(15) << "|Tensi Darah" << endl;
//     cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;
//     cout << setw(15) << "|" << setw(15) << "|" << setw(15) << "|"
//          << setw(15) << "|Sistolik"
//          << setw(15) << "|Diastolik" << endl;
//     cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;

//     while (pBantu != NULL)
//     {
//         cout << "|" << setw(14) << pBantu->pemeriksaanAwal.beratBadan << "|" << setw(14) << pBantu->pemeriksaanAwal.tinggiBadan
//              << "|" << setw(14) << pBantu->pemeriksaanAwal.suhuBadan << "|" << setw(14) << pBantu->pemeriksaanAwal.tekananDarah.sistolik
//              << "|" << setw(14) << pBantu->pemeriksaanAwal.tekananDarah.diastolik << endl;
//         pBantu = pBantu->next;
//     }
//     cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;
// }