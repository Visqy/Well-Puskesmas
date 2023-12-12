#include <string>
using namespace std;

// Queue
struct PasienAntrianPendaftaran
{
    string nama;
    string pilihPoliklinik;
    bool isOnline;
    bool isDequeue;

    // operasi periksa apakah sama
    bool operator==(const PasienAntrianPendaftaran &other) const
    {
        return nama == other.nama && isOnline == other.isOnline && pilihPoliklinik == other.pilihPoliklinik;
    }
};

struct pasienQueue
{
    string pilihanPoliklinik;
    int nomorPasien;
    bool isOnline, isDequeue;
};

// Multi List
struct treatment
{
    string namaTreatment;
    long biaya;
    treatment *next;
};

struct diagnosa
{
    string hasilDiagnosa, waktuDiagnosa;
    treatment *treatmentDiagnosa;
    diagnosa *next;
};

struct riwayattransaksipasien
{
    int nomorTransaksi;
    string waktuTransaksi;
    long total;
    riwayattransaksipasien *next;
};

struct dataPribadi
{
    string nama, alamat, jenisKelamin, tempatLahir, tanggalLahir, kontak;
};

struct tensiDarah
{
    int sistolik, diastolik;
};

struct pemeriksaanAwal
{
    int beratBadan, tinggiBadan, suhuBadan;
    tensiDarah tekananDarah;
};

struct pasien
{
    int nomor;
    dataPribadi dataPribadi;
    pemeriksaanAwal pemeriksaanAwal;
    // database
    diagnosa *diagnosaPasien;
    riwayattransaksipasien *riwayatTransaksi;
    pasien *next;
};

struct riwayattransaksi
{
    int nomorTransaksi, idPasien;
    string waktuTransaksi, namaPasien;
    Treatment treatment;
    long total;
    riwayattransaksi *next;
};

typedef riwayattransaksi *riwayatTransaksi;
typedef diagnosa *Diagnosa;
typedef treatment *Treatment;
typedef riwayattransaksipasien *riwayatTransaksiPasien;
typedef pasien *Pasien;
