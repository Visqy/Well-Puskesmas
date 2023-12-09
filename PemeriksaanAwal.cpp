#include<iostream>
#include<iomanip>
using namespace std;

struct PemeriksaanAwal {
    int beratbadan, tinggibadan, suhubadan, tensidarah;
};

struct node {
    PemeriksaanAwal data;
    node* next;
};

typedef node* pointernode;

void createElementPemeriksaan(pointernode& pBaru) {
    pBaru = new node;
    cin.ignore();
    cout << "Masukkan Berat Badan   : "; cin >> pBaru->data.beratbadan;
    cout << "Masukkan Tinggi Badan  : "; cin >> pBaru->data.tinggibadan;
    cout << "Masukkan Suhu Badan    : "; cin >> pBaru->data.suhubadan;
    cout << "Masukkan Tensi Darah   : "; cin >> pBaru->data.tensidarah;
    cout << endl;
    pBaru->next = NULL;
}

void displayOutputPemeriksaan(pointernode& first) {
    pointernode pBantu = first;
    cout << "Data Pemeriksaan Awal" << endl;
    cout << setfill('-') << setw(60) << "" << setfill(' ') << endl;
    cout << setw(15) << left << "Berat Badan"
         << setw(15) << "Tinggi Badan"
         << setw(15) << "Suhu Badan"
         << setw(15) << "Tensi Darah" << endl;
    cout << setfill('-') << setw(60) << "" << setfill(' ') << endl;

    while (pBantu != NULL) {
        cout << setw(15) << pBantu->data.beratbadan << setw(15) << pBantu->data.tinggibadan
             << setw(15) << pBantu->data.suhubadan << setw(15) << pBantu->data.tensidarah << endl;
        pBantu = pBantu->next;
    }
    cout << setfill('-') << setw(60) << "" << setfill(' ') << endl;
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
