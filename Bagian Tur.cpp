#include <iostream>
#include <iomanip>
#include <cstring>
#include <ctime>

using namespace std;

// Struktur data untuk hasil diagnosa
struct Diagnosa {
    string hasilDiagnosa;
    string resepObat;
};

// Node untuk multi-linked list pemeriksaan
struct PemeriksaanNode {
    int nomorPemeriksaan;
    Diagnosa diagnosa;
    PemeriksaanNode* nextPemeriksaan;
};

// Node untuk linked list pembayaran
struct PembayaranNode {
    int nomorTransaksi;
    double totalBiaya;
    PembayaranNode* nextTransaksi;
};

// Queue untuk pencatatan transaksi pembayaran
class TransaksiQueue {
private:
    PembayaranNode* front;
    PembayaranNode* rear;

public:
    TransaksiQueue() : front(nullptr), rear(nullptr) {}

    void enqueue(int nomorTransaksi, double totalBiaya) {
        PembayaranNode* newNode = new PembayaranNode;
        newNode->nomorTransaksi = nomorTransaksi;
        newNode->totalBiaya = totalBiaya;
        newNode->nextTransaksi = nullptr;

        if (rear == nullptr) {
            front = rear = newNode;
        } else {
            rear->nextTransaksi = newNode;
            rear = newNode;
        }
    }

    void dequeue() {
        if (front == nullptr) {
            cout << "Antrian transaksi kosong.\n";
            return;
        }

        PembayaranNode* temp = front;
        front = front->nextTransaksi;

        delete temp;
    }

    // Traversal struktur pembayaran
    void traversePembayaran() {
        if (front == nullptr) {
            cout << "Daftar transaksi pembayaran kosong.\n";
            return;
        }

        cout << "\n==============================================\n";
        cout << setw(20) << "Nomor Transaksi" << setw(20) << "Total Biaya" << "\n";
        cout << "==============================================\n";

        PembayaranNode* current = front;
        while (current != nullptr) {
            cout << setw(20) << current->nomorTransaksi << setw(20) << fixed << setprecision(2) << current->totalBiaya << "\n";
            current = current->nextTransaksi;
        }

        cout << "==============================================\n";
    }
};

// Kelas untuk multi-linked list pemeriksaan
class PemeriksaanList {
private:
    PemeriksaanNode* head;

public:
    PemeriksaanList() : head(nullptr) {}

    // Menambahkan pemeriksaan baru
    void tambahPemeriksaan(int nomorPemeriksaan, Diagnosa diagnosa) {
        PemeriksaanNode* newNode = new PemeriksaanNode;
        newNode->nomorPemeriksaan = nomorPemeriksaan;
        newNode->diagnosa = diagnosa;
        newNode->nextPemeriksaan = nullptr;

        if (head == nullptr) {
            head = newNode;
        } else {
            PemeriksaanNode* current = head;
            while (current->nextPemeriksaan != nullptr) {
                current = current->nextPemeriksaan;
            }
            current->nextPemeriksaan = newNode;
        }
    }

    // Traversal multi-linked list pemeriksaan
    void traversePemeriksaan() {
        if (head == nullptr) {
            cout << "Daftar pemeriksaan kosong.\n";
            return;
        }

        cout << "\n==============================================\n";
        cout << setw(20) << "Nomor Pemeriksaan" << setw(20) << "Hasil Diagnosa" << setw(20) << "Resep Obat" << "\n";
        cout << "==============================================\n";

        PemeriksaanNode* current = head;
        while (current != nullptr) {
            cout << setw(20) << current->nomorPemeriksaan << setw(20) << current->diagnosa.hasilDiagnosa << setw(20) << current->diagnosa.resepObat << "\n";
            current = current->nextPemeriksaan;
        }

        cout << "==============================================\n";
    }
};

void cetakNota(PemeriksaanList &pemeriksaanList, TransaksiQueue &transaksiQueue) {
    // Informasi Puskesmas
    cout << "NOTA PEMERIKSAAN KESEHATAN\n\n";
    cout << "WELL PUSKESMAS\n";
    cout << "------------------------------\n";
    cout << "Jalan Raya Bandung – Sumedang KM. 21 Jatinangor 45361, Sumedang, Jawa Barat\n\n";

    // Informasi pasien (harap disesuaikan dengan data pasien yang sebenarnya)
    cout << "No. Rawat  : 001\n";
    cout << "Tgl. Rawat : 01/01/2023\n";
    cout << "No. RM     : 123456\n";
    cout << "Nama Pasen : John Doe\n\n";

    // Informasi tanggal cetak nota
    time_t now = time(0);
    char* dt = ctime(&now);
    cout << setw(60) << "Tgl. Cetak : " << dt;

    // Daftar Tindakan Pasien
    cout << "\nDAFTAR TINDAKAN PASIEN\n";
    cout << "============================================================================================\n";
    cout << setw(15) << "TANGGAL" << setw(30) << "TINDAKAN" << setw(30) << "DOKTER" << setw(20) << "BIAYA\n";
    cout << "============================================================================================\n";

    // (Implementasikan traversal dan pencetakan daftar tindakan pasien di sini)

    cout << "============================================================================================\n";
    cout << setw(65) << "TOTAL TINDAKAN (Rp) :   \n";
    cout << "\n";

    // Daftar Obat Pasien
    cout << "DAFTAR OBAT PASIEN\n";
    cout << "============================================================================================\n";
    cout << setw(15) << "TANGGAL" << setw(40) << "KODE/ NAMA OBAT" << setw(20) << "JUMLAH" << setw(20) << "HARGA\n";
    cout << "============================================================================================\n";

    // (Implementasikan traversal dan pencetakan daftar obat pasien di sini)

    cout << "============================================================================================\n";
    cout << setw(65) << "TOTAL OBAT (Rp) :   \n";
    cout << "\n";

    // Ringkasan Biaya
    cout << "--------------------------------------------------------------------------------------------\n";
    cout << setw(28) << "TOTAL TINDAKAN (Rp) :   \n";
    cout << setw(28) << "TOTAL OBAT     (Rp) :   \n";
    cout << "-------------------------------- (+)\n";
    cout << setw(28) << "BIAYA TOTAL    (Rp) :   \n";
    cout << setw(28) << "UANG MUKA      (Rp) :   \n";
    cout << "================================ (-)\n";
    cout << setw(28) << "UANG KEMBALU   (Rp) :   \n";
    cout << "--------------------------------------------------------------------------------------------\n";
}

int main() {
    // Inisialisasi multi-linked list pemeriksaan
    PemeriksaanList pemeriksaanList;

    // Menambahkan beberapa pemeriksaan
    pemeriksaanList.tambahPemeriksaan(1, {"Flu", "Obat A"});
    pemeriksaanList.tambahPemeriksaan(2, {"Demam", "Obat B"});

    // Menampilkan daftar pemeriksaan
    pemeriksaanList.traversePemeriksaan();

    // Inisialisasi queue transaksi pembayaran
    TransaksiQueue transaksiQueue;

    // Menambahkan transaksi pembayaran
    transaksiQueue.enqueue(101, 150.0);
    transaksiQueue.enqueue(102, 200.5);

    // Menampilkan daftar transaksi pembayaran
    transaksiQueue.traversePembayaran();

    // Menghapus transaksi dari antrian
    transaksiQueue.dequeue();
    transaksiQueue.traversePembayaran();

    // Mencetak nota pembayaran
    cetakNota(pemeriksaanList, transaksiQueue);

    return 0;
}
