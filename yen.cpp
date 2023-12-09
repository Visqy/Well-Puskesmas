#include <iostream>
#include <string>
#include "queue.cpp"

using namespace std;

struct Node {
    string data;
    Node* next;
};

// Node untuk antrian pasien pada setiap poliklinik
struct NodePasien {
    string namaPasien;
    int nomorPasien;
};
/*
// Antrian untuk setiap poliklinik
struct AntrianPoliklinik {
    NodePasien* depan;
    NodePasien* belakang;
};


Node* buatNode(string data) {
    Node* nodeBaru = new Node;
    nodeBaru->data = data;
    nodeBaru->next = NULL;
    return nodeBaru;
}


NodePasien* buatNodePasien(string namaPasien) {
    NodePasien* nodePasienBaru = new NodePasien;
    nodePasienBaru->namaPasien = namaPasien;
    nodePasienBaru->next = NULL;
    return nodePasienBaru;
}


void tambahNode(Node*& kepala, string data) {
    Node* nodeBaru = buatNode(data);
    if (kepala == NULL) {
        kepala = nodeBaru;
    } else {
        Node* temp = kepala;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = nodeBaru;
    }
}

void tambahPasien(AntrianPoliklinik& antrian, string namaPasien) {
    NodePasien* pasienBaru = buatNodePasien(namaPasien);
    if (antrian.depan == NULL) {
        antrian.depan = pasienBaru;
        antrian.belakang = pasienBaru;
    } else {
        antrian.belakang->next = pasienBaru;
        antrian.belakang = pasienBaru;
    }
    cout << "Pasien berhasil ditambahkan." << endl;
}

void panggilPasien(AntrianPoliklinik& antrian) {
    if (antrian.depan == NULL) {
        cout << "Antrian kosong." << endl;
    } else {
        NodePasien* temp = antrian.depan;
        antrian.depan = antrian.depan->next;
        delete temp;
        cout << "Pasien dipanggil dan dihapus dari antrian." << endl;
    }
}

void tampilkanAntrian(AntrianPoliklinik antrian, string namaPoliklinik) {
    cout << "Antrian Poliklinik " << namaPoliklinik << ": ";
    NodePasien* temp = antrian.depan;
    while (temp != NULL) {
        cout << temp->namaPasien << " ";
        temp = temp->next;
    }
    cout << endl;
}
*/
int main() {
    Queue<NodePasien> antrianUmum;
    Queue<NodePasien> antrianTHT;
    Queue<NodePasien> antrianGigi;
    Queue<NodePasien> antrianMata;
    string namaPasien;
    NodePasien newPasien;

    int pilihan;

    do {
        cout << "\nMenu Pemilihan Poliklinik:" << endl;
        cout << "1. Poliklinik Umum" << endl;
        cout << "2. Poliklinik THT" << endl;
        cout << "3. Poliklinik Gigi" << endl;
        cout << "4. Poliklinik Mata" << endl;
        cout << "5. Keluar" << endl;

        cout << "Pilih poliklinik (1-5): ";
        cin >> pilihan;

        switch (pilihan) {
            case 1:
                cout << "Masukkan nama pasien: ";
                cin.ignore();
                getline(cin, namaPasien);
                newPasien.namaPasien = (namaPasien);
                antrianUmum.enqueue(newPasien);
                break;

            case 2:
                cout << "Masukkan nama pasien: ";
                cin.ignore();
                getline(cin, namaPasien);
                newPasien.namaPasien = (namaPasien);
                antrianTHT.enqueue(newPasien);
                break;

            case 3:
                cout << "Masukkan nama pasien: ";
                cin.ignore();
                getline(cin, namaPasien);
                newPasien.namaPasien = (namaPasien);
                antrianGigi.enqueue(newPasien);
                break;

            case 4:
                cout << "Masukkan nama pasien: ";
                cin.ignore();
                getline(cin, namaPasien);
                newPasien.namaPasien = (namaPasien);
                antrianMata.enqueue(newPasien);
                break;

            case 5:
                cout << "Terima kasih. Keluar dari program." << endl;
                break;

            default:
                cout << "Pilihan tidak valid." << endl;
        }
    } while (pilihan != 0);

    return 0;
}
