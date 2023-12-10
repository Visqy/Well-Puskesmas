#include<iostream>
#include<iomanip>
using namespace std;

struct tensiDarah{
    float sistolik, diastolik;
};

struct pemeriksaanAwal {
    float beratbadan, tinggibadan, suhubadan;
    tensiDarah tds;
};

struct node {
    pemeriksaanAwal data;
    node* next;
};

typedef node* pointernode;

void createElementPemeriksaan(pointernode& pBaru) {
    pBaru = new node;
    cin.ignore();
    cout << "Masukkan Berat Badan       : "; cin >> pBaru->data.beratbadan;
    cout << "Masukkan Tinggi Badan      : "; cin >> pBaru->data.tinggibadan;
    cout << "Masukkan Suhu Badan        : "; cin >> pBaru->data.suhubadan;
    cout << "Masukkan Nilai Sistolik    : "; cin >> pBaru->data.tds.sistolik;
    cout << "Masukkan Nilai Diastolik   : "; cin >> pBaru->data.tds.diastolik;
    cout << endl;
    pBaru->next = NULL;
}

void displayOutputPemeriksaan(pointernode& first) {
    pointernode pBantu = first;
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

    while (pBantu != NULL) {
        cout << "|" << setw(14) << pBantu->data.beratbadan << "|" << setw(14) << pBantu->data.tinggibadan
             << "|" << setw(14) << pBantu->data.suhubadan << "|" << setw(14) << pBantu->data.tds.sistolik
             << "|" << setw(14) << pBantu->data.tds.diastolik << endl;
        pBantu = pBantu->next;
    }
    cout << setfill('-') << setw(80) << "" << setfill(' ') << endl;
}

int main() {
    pointernode first = NULL, pBaru = NULL;
    cout << "=== Pemeriksaan Awal ===" << endl;
    createElementPemeriksaan(pBaru);
    if (first == NULL) {
        first = pBaru;
    }
    displayOutputPemeriksaan(first);
    delete pBaru;

    return 0;
}
