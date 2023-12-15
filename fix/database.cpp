#include <iostream>
#include <sqlite3.h>
#include <string>
#include <fstream>
#include <vector>
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
void insertLastPasien(Pasien &first, Pasien pBantu)
{
    if (first == NULL)
    {
        first = pBantu;
    }
    else
    {
        Pasien currentPasien = first;
        while (currentPasien->next != NULL)
        {
            currentPasien = currentPasien->next;
        }
        currentPasien->next = pBantu;
    }
}
void insertLastRiwayatTransaksi(riwayatTransaksi &first, riwayatTransaksi pBantu)
{
    if (first == NULL)
    {
        first = pBantu;
    }
    else
    {
        riwayatTransaksi currentRiwayat = first;
        while (currentRiwayat->next != NULL)
        {
            currentRiwayat = currentRiwayat->next;
        }
        currentRiwayat->next = pBantu;
    }
}

// Fungsi untuk menambahkan diagnosa ke pasien tertentu
void insertLastDiagnosaPasien(Pasien first, int nomorPasien, Diagnosa dataDiagnosa)
{
    Pasien currentPasien = first;
    while (currentPasien != NULL && currentPasien->nomor != nomorPasien)
    {
        currentPasien = currentPasien->next;
    }

    if (currentPasien != NULL)
    {
        dataDiagnosa->next = currentPasien->diagnosaPasien;
        currentPasien->diagnosaPasien = dataDiagnosa;
    }
}

// Fungsi untuk menambahkan treatment ke diagnosa tertentu
void insertLastTreatmentPasien(Pasien &first, int nomorPasien, int idDiagnosa, Treatment dataTreatment)
{
    Pasien currentPasien = first;
    while (currentPasien != NULL && currentPasien->nomor != nomorPasien)
    {
        currentPasien = currentPasien->next;
    }

    if (currentPasien != NULL)
    {
        Diagnosa currentDiagnosa = currentPasien->diagnosaPasien;
        while (currentDiagnosa != NULL && currentDiagnosa->id != idDiagnosa)
        {
            currentDiagnosa = currentDiagnosa->next;
        }

        if (currentDiagnosa != NULL)
        {
            dataTreatment->next = currentDiagnosa->treatmentDiagnosa;
            currentDiagnosa->treatmentDiagnosa = dataTreatment;
        }
    }
}
// Fungsi untuk menambahkan data pribadi ke pasien tertentu
void addDataPribadiPasien(Pasien &first, int nomorPasien, dataPribadi dataPasien)
{
    Pasien currentPasien = first;
    while (currentPasien != NULL && currentPasien->nomor != nomorPasien)
    {
        currentPasien = currentPasien->next;
    }

    if (currentPasien != NULL)
    {
        currentPasien->dataPribadi = dataPasien;
    }
}

// Fungsi untuk menambahkan pemeriksaan awal ke pasien tertentu
void addPemeriksaanAwalPasien(Pasien &first, int nomorPasien, pemeriksaanAwal dataPemeriksaanAwal)
{
    Pasien currentPasien = first;
    while (currentPasien != NULL && currentPasien->nomor != nomorPasien)
    {
        currentPasien = currentPasien->next;
    }

    if (currentPasien != NULL)
    {
        currentPasien->pemeriksaanAwal = dataPemeriksaanAwal;
    }
}
void duplicateTreatmentToTransaksi(Pasien first, riwayatTransaksi &firstRiwayat, int nomorPasien, int idDiagnosa)
{
    Pasien currentPasien = first;
    while (currentPasien != NULL && currentPasien->nomor != nomorPasien)
    {
        currentPasien = currentPasien->next;
    }

    if (currentPasien != NULL)
    {
        // Cari diagnosa dengan id yang sesuai
        Diagnosa currentDiagnosa = currentPasien->diagnosaPasien;
        while (currentDiagnosa != NULL && currentDiagnosa->id != idDiagnosa)
        {
            currentDiagnosa = currentDiagnosa->next;
        }

        if (currentDiagnosa != NULL)
        {
            // Duplicate treatment ke riwayat transaksi
            Treatment currentTreatment = currentDiagnosa->treatmentDiagnosa;
            while (currentTreatment != NULL)
            {
                Treatment duplicatedTreatment = new treatment(*currentTreatment); // Duplicate treatment
                duplicatedTreatment->next = firstRiwayat->treatmentPasien;
                firstRiwayat->treatmentPasien = duplicatedTreatment;

                currentTreatment = currentTreatment->next;
            }
        }
    }
}

// convert any struct to string for transfer data
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
class DatabaseManager
{
private:
    sqlite3 *db;
    // Private constructor for singleton pattern
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

    // Private destructor to close the database connection
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
            "    username TEXT PRIMARY KEY NOT NULL,"
            "    password TEXT NOT NULL,"
            "    usertype INTEGER NOT NULL"
            ");";

        const char *dokter =
            "CREATE TABLE IF NOT EXISTS dokter ("
            "    username TEXT PRIMARY KEY NOT NULL,"
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

    bool readDataPasien(Pasien &first)
    {
        int rc;

        const char *sql = "SELECT * FROM pasien;";
        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;

            return false;
        }

        first = NULL;

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            Pasien pPasien = new pasien;
            pPasien->nomor = sqlite3_column_int(stmt, 0);

            pPasien->dataPribadi.nama = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            pPasien->dataPribadi.alamat = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            pPasien->dataPribadi.jenisKelamin = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
            pPasien->dataPribadi.tempatLahir = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
            pPasien->dataPribadi.tanggalLahir = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
            pPasien->dataPribadi.kontak = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

            // TODO: Retrieve and connect other data (diagnosaPasien)
            const char *sqlDiagnosa = "SELECT * FROM diagnosa WHERE pasien_nomor = ?;";
            sqlite3_stmt *stmtDiagnosa;

            rc = sqlite3_prepare_v2(db, sqlDiagnosa, -1, &stmtDiagnosa, 0);

            if (rc != SQLITE_OK)
            {
                cerr << "Cannot prepare statement for diagnosaPasien: " << sqlite3_errmsg(db) << endl;

                return false;
            }

            sqlite3_bind_int(stmtDiagnosa, 1, pPasien->nomor);
            pPasien->diagnosaPasien = NULL;
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
                    else
                    {
                        delete pTreatment;
                    }
                }
                sqlite3_finalize(stmtTreatment);
                if (pPasien->nomor == pDiagnosa->id)
                {
                    insertLastDiagnosa(pPasien->diagnosaPasien, pDiagnosa);
                }
                else
                {
                    delete pDiagnosa;
                }
            }
            sqlite3_finalize(stmtDiagnosa);
            pPasien->next = NULL;
            insertLastPasien(first, pPasien);
        }

        sqlite3_finalize(stmt);

        return true;
    }

    bool savePasienOnExit(Pasien first)
    {
        int rc;
        // Save patient data to the database
        const char *sqlPasien = "INSERT INTO pasien (nama, alamat, jenisKelamin, tempatLahir, tanggalLahir, kontak) VALUES (?, ?, ?, ?, ?, ?);";
        sqlite3_stmt *stmtPasien;
        rc = sqlite3_prepare_v2(db, sqlPasien, -1, &stmtPasien, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement for inserting pasien: " << sqlite3_errmsg(db) << endl;
            return false;
        }
        sqlite3_bind_text(stmtPasien, 1, first->dataPribadi.nama.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtPasien, 2, first->dataPribadi.alamat.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtPasien, 3, first->dataPribadi.jenisKelamin.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtPasien, 4, first->dataPribadi.tempatLahir.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtPasien, 5, first->dataPribadi.tanggalLahir.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtPasien, 6, first->dataPribadi.kontak.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmtPasien);
        if (rc != SQLITE_DONE)
        {
            cerr << "Execution failed: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmtPasien);
            return false;
        }
        int IDpasien = sqlite3_last_insert_rowid(db);

        sqlite3_finalize(stmtPasien);

        // Save diagnosis and treatment data to the database
        Diagnosa currentDiagnosa = first->diagnosaPasien;
        while (currentDiagnosa != NULL)
        {
            const char *sqlDiagnosa = "INSERT INTO diagnosa (hasilDiagnosa, waktuDiagnosa, pasien_nomor) VALUES (?, ?, ?);";
            sqlite3_stmt *stmtDiagnosa;
            rc = sqlite3_prepare_v2(db, sqlDiagnosa, -1, &stmtDiagnosa, 0);

            if (rc != SQLITE_OK)
            {
                cerr << "Cannot prepare statement for inserting diagnosa: " << sqlite3_errmsg(db) << endl;

                return false;
            }

            sqlite3_bind_text(stmtDiagnosa, 1, currentDiagnosa->hasilDiagnosa.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmtDiagnosa, 2, currentDiagnosa->waktuDiagnosa.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmtDiagnosa, 3, IDpasien);

            rc = sqlite3_step(stmtDiagnosa);
            if (rc != SQLITE_DONE)
            {
                cerr << "Execution failed: " << sqlite3_errmsg(db) << endl;
                sqlite3_finalize(stmtDiagnosa);

                return false;
            }
            int lastInsertID = sqlite3_last_insert_rowid(db);
            sqlite3_finalize(stmtDiagnosa);

            // Save treatment data for this diagnosis
            Treatment currentTreatment = currentDiagnosa->treatmentDiagnosa;
            while (currentTreatment != NULL)
            {
                const char *sqlTreatment = "INSERT INTO treatment (namaTreatment, biaya, diagnosa_id) VALUES (?, ?, ?);";
                sqlite3_stmt *stmtTreatment;
                rc = sqlite3_prepare_v2(db, sqlTreatment, -1, &stmtTreatment, 0);

                if (rc != SQLITE_OK)
                {
                    cerr << "Cannot prepare statement for inserting treatment: " << sqlite3_errmsg(db) << endl;

                    return false;
                }

                sqlite3_bind_text(stmtTreatment, 1, currentTreatment->namaTreatment.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_int(stmtTreatment, 2, currentTreatment->biaya);
                sqlite3_bind_int(stmtTreatment, 3, lastInsertID);

                rc = sqlite3_step(stmtTreatment);
                if (rc != SQLITE_DONE)
                {
                    cerr << "Execution failed: " << sqlite3_errmsg(db) << endl;
                    sqlite3_finalize(stmtTreatment);

                    return false;
                }

                sqlite3_finalize(stmtTreatment);

                currentTreatment = currentTreatment->next;
            }

            currentDiagnosa = currentDiagnosa->next;
        }

        return true;
    }

    // Riwayat transaksi
    bool readRiwayatTransaksi(riwayattransaksi *&firstRiwayat)
    {
        int rc;

        const char *sql = "SELECT * FROM riwayattransaksi;";
        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement for riwayattransaksi: " << sqlite3_errmsg(db) << endl;

            return false;
        }

        firstRiwayat = NULL;

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            riwayattransaksi *pRiwayat = new riwayattransaksi;
            pRiwayat->nomorTransaksi = sqlite3_column_int(stmt, 0);
            pRiwayat->idPasien = sqlite3_column_int(stmt, 1);
            pRiwayat->idDiagnosa = sqlite3_column_int(stmt, 2);
            pRiwayat->waktuTransaksi = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
            pRiwayat->namaPasien = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
            pRiwayat->total = sqlite3_column_int(stmt, 5);
            pRiwayat->jumlahDibayarkan = sqlite3_column_int(stmt, 6);

            // TODO: Retrieve and connect treatment data for this transaction
            const char *sqlTreatment = "SELECT * FROM treatment WHERE diagnosa_id = ?;";
            sqlite3_stmt *stmtTreatment;

            rc = sqlite3_prepare_v2(db, sqlTreatment, -1, &stmtTreatment, 0);

            if (rc != SQLITE_OK)
            {
                cerr << "Cannot prepare statement for treatment: " << sqlite3_errmsg(db) << endl;

                return false;
            }

            sqlite3_bind_int(stmtTreatment, 1, pRiwayat->idDiagnosa);
            pRiwayat->treatmentPasien = NULL;
            while ((rc = sqlite3_step(stmtTreatment)) == SQLITE_ROW)
            {
                treatment *pTreatment = new treatment;
                pTreatment->id = sqlite3_column_int(stmtTreatment, 0);
                pTreatment->namaTreatment = reinterpret_cast<const char *>(sqlite3_column_text(stmtTreatment, 1));
                pTreatment->biaya = sqlite3_column_int(stmtTreatment, 2);
                pTreatment->next = NULL;

                if (pTreatment->id == pRiwayat->idDiagnosa)
                {
                    insertLastTreatment(pRiwayat->treatmentPasien, pTreatment);
                }
                else
                {
                    delete pTreatment;
                }
            }
            sqlite3_finalize(stmtTreatment);

            pRiwayat->next = NULL;
            insertLastRiwayatTransaksi(firstRiwayat, pRiwayat);
        }

        sqlite3_finalize(stmt);

        return true;
    }

    bool saveRiwayatTransaksiOnExit(riwayattransaksi *firstRiwayat)
    {
        int rc;
        // Iterate over the linked list and save each riwayattransaksi to the database
        riwayattransaksi *currentRiwayat = firstRiwayat;
        while (currentRiwayat != NULL)
        {
            // Save riwayattransaksi data to the database
            const char *sqlRiwayat = "INSERT INTO riwayattransaksi (idDiagnosa, idPasien, waktuTransaksi, namaPasien, total, jumlahDibayarkan) VALUES (?,?, ?, ?, ?, ?);";
            sqlite3_stmt *stmtRiwayat;
            rc = sqlite3_prepare_v2(db, sqlRiwayat, -1, &stmtRiwayat, 0);

            if (rc != SQLITE_OK)
            {
                cerr << "Cannot prepare statement for inserting riwayattransaksi: " << sqlite3_errmsg(db) << endl;

                return false;
            }

            sqlite3_bind_int(stmtRiwayat, 1, currentRiwayat->idDiagnosa);
            sqlite3_bind_int(stmtRiwayat, 2, currentRiwayat->idPasien);
            sqlite3_bind_text(stmtRiwayat, 3, currentRiwayat->waktuTransaksi.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmtRiwayat, 4, currentRiwayat->namaPasien.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmtRiwayat, 5, currentRiwayat->total);
            sqlite3_bind_int(stmtRiwayat, 6, currentRiwayat->jumlahDibayarkan);

            rc = sqlite3_step(stmtRiwayat);
            if (rc != SQLITE_DONE)
            {
                cerr << "Execution failed: " << sqlite3_errmsg(db) << endl;
                sqlite3_finalize(stmtRiwayat);

                return false;
            }

            sqlite3_finalize(stmtRiwayat);
            currentRiwayat = currentRiwayat->next;
        }

        return true;
    }

    // Function to verify user credentials
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
            userType = static_cast<UserType>(sqlite3_column_int(stmt, 2));
            sqlite3_finalize(stmt);

            return true;
        }

        sqlite3_finalize(stmt);

        return false;
    }

    // Function to update userdata by username
    bool updateUserdataByUsername(string username, UserData userdata)
    {
        int rc;
        const char *sql = "UPDATE userdata SET password = ? WHERE username = ?;";
        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;

            return false;
        }

        sqlite3_bind_text(stmt, 1, userdata.password.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);

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

    // Function to update dokter by username
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

    // Function to get information about a dokter by username
    void getInformasiDokter(string username, Dokter &dataDokter)
    {
        int rc;

        const char *sql = "SELECT * FROM dokter WHERE username = ?;";
        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW)
        {
            dataDokter.password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
            dataDokter.nama = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            dataDokter.poliklinik = static_cast<Poliklinik>(sqlite3_column_int(stmt, 2));
            dataDokter.username = username;
        }

        sqlite3_finalize(stmt);
    }

    // Function to delete userdata by username
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

    // Function to delete dokter by username
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

    // Function to read all dokter data
    void readDokter(vector<Dokter> &dataAllDokter)
    {
        int rc;

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        const char *sql = "SELECT * FROM dokter;";
        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK)
        {
            cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Dokter dokter;
            dokter.password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
            dokter.nama = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            dokter.poliklinik = static_cast<Poliklinik>(sqlite3_column_int(stmt, 2));
            dokter.username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));

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
            dataDokter.nama = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
            dataDokter.poliklinik = static_cast<Poliklinik>(sqlite3_column_int(stmt, 1));
            dataDokter.username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            dataDokter.password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));

            sqlite3_finalize(stmt);
            return true;
        }

        sqlite3_finalize(stmt);
        return false;
    }

    void readUsername(vector<UserData> &dataAllUsername)
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
            userData.username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
            userData.password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            userData.usertype = static_cast<UserType>(sqlite3_column_int(stmt, 2));

            dataAllUsername.push_back(userData);
        }

        sqlite3_finalize(stmt);
    }

    bool addDokter(Dokter &dataDokter)
    {
        const char *sqlDokter = "INSERT INTO dokter (nama, poliklinik, username, password) VALUES (?, ?, ?, ?);";
        sqlite3_stmt *stmtDokter;
        int rc = sqlite3_prepare_v2(db, sqlDokter, -1, &stmtDokter, 0);

        if (rc != SQLITE_OK)
        {
            std::cerr << "Cannot prepare statement for inserting dokter: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_text(stmtDokter, 1, dataDokter.nama.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmtDokter, 2, static_cast<int>(dataDokter.poliklinik));
        sqlite3_bind_text(stmtDokter, 3, dataDokter.username.c_str(), -1, SQLITE_STATIC);
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

    bool addUserdata(UserData userdata)
    {
        const char *sqlUserdata = "INSERT INTO userdata (username, password, usertype) VALUES (?, ?, ?);";
        sqlite3_stmt *stmtUserdata;
        int rc = sqlite3_prepare_v2(db, sqlUserdata, -1, &stmtUserdata, 0);

        if (rc != SQLITE_OK)
        {
            std::cerr << "Cannot prepare statement for inserting userdata: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        sqlite3_bind_text(stmtUserdata, 1, userdata.username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmtUserdata, 2, userdata.password.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmtUserdata, 3, static_cast<int>(userdata.usertype));

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
            int count = sqlite3_column_int(stmt, 0);
            result = (count > 0);
        }

        sqlite3_finalize(stmt);
        return result;
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
            usertype = static_cast<UserType>(sqlite3_column_int(stmt, 0));
            result = true;
        }

        sqlite3_finalize(stmt);
        return result;
    }
};

void displayOutputPemeriksaan(Pasien first)
{
    Pasien pBantu = first;
    cout << "Data Pemeriksaan Awal" << endl;
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

    while (pBantu != NULL)
    {
        cout << "|" << setw(14) << pBantu->pemeriksaanAwal.beratBadan << "|" << setw(14) << pBantu->pemeriksaanAwal.tinggiBadan
             << "|" << setw(14) << pBantu->pemeriksaanAwal.suhuBadan << "|" << setw(14) << pBantu->pemeriksaanAwal.tekananDarah.sistolik
             << "|" << setw(14) << pBantu->pemeriksaanAwal.tekananDarah.diastolik << endl;
        pBantu = pBantu->next;
    }
    cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;
}