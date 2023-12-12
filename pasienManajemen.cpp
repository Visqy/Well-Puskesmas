#include <iostream>
#include <iomanip>
using namespace std;

struct pasien
{
    int nomor_pasien, tgllahir;
    char nama_pasien[50];
    char riwayat[100];
    pasien* next;
    pasien* prev;
};

typedef pasien* pointer;
typedef pointer list;

void createList(list& first)
{
    first = NULL;
}

void createElmt(pointer& pBaru)
{
    pBaru = new pasien;
    cout << "\nMasukkan nomor pasien            : ";
    cin >> pBaru->nomor_pasien;
    cin.ignore();
    cout << "Masukkan nama pasien             : ";
    cin.getline(pBaru->nama_pasien, 50);
    cout << "Masukkan tanggal lahir pasien    : ";
    cin >> pBaru->tgllahir;
    cin.ignore();
    cout << "Masukkan riwayat medis pasien    : ";
    cin.getline(pBaru->riwayat, 100);
    pBaru->next = NULL;
}

void traversal(list first)
{
    pointer pBantu = first;
    if (pBantu == NULL)
    {
        cout << "Tidak ada data pasien yang dapat ditemukan" << endl;
        return;
    }

    cout << "-------------------------------------------------------------" << endl;
    cout << "DAFTAR PASIEN WELL PUSKESMAS                         " << endl;
    cout << "-------------------------------------------------------------" << endl;
    cout << left << setw(5) << "No" << setw(15) << "Nomor Pasien" << setw(20) << "Nama Pasien" << setw(10) << "Tanggal Lahir Pasien" << setw(25) << "Riwayat Medis" << setw(25) << endl;

    int i = 1;

    while (pBantu != NULL)
    {
        cout << left << setw(5) << i << setw(15) << pBantu->nomor_pasien << setw(20) << pBantu->nama_pasien << setw(10) << pBantu->tgllahir << setw(25) << pBantu->riwayat << setw(25) << endl;
        pBantu = pBantu->next;
        i++;
    }
}

void InsertFirst(list& first, pointer& pBaru)
{
    if (first == NULL)
        first = pBaru;
    else
    {
        pBaru->next = first;
        first = pBaru;
    }
}

void InsertLast(list& first, pointer& pBaru)
{
    pointer last;
    if (first == NULL)
        first = pBaru;
    else
    {
        last = first;
        while (last->next != NULL)
        {
            last = last->next;
        }
        last->next = pBaru;
    }
}

void DeleteFirst(list& first, pointer& pHapus)
{
    if (first == NULL)
    {
        pHapus = NULL;
        cout << "Tidak ada data pasien yang dapat dihapus" << endl;
    }
    else if (first->next == NULL)
    {
        pHapus = first;
        first = NULL;
    }
    else
    {
        pHapus = first;
        first = first->next;
        pHapus->next = NULL;
    }
}

void DeleteLast(list& first, pointer& pHapus)
{
    pointer last, prelast;
    if (first == NULL)
    {
        pHapus = NULL;
        cout << "Tidak ada data pasien yang dapat dihapus" << endl;
    }
    else if (first->next == NULL)
    {
        pHapus = first;
        first = NULL;
    }
    else
    {
        last = first;
        prelast = NULL;
        while (last->next != NULL)
        {
            prelast = last;
            last = last->next;
        }
        pHapus = last;
        prelast->next = NULL;
    }
}

pointer search(list first, int key)
{
    pointer pBantu = first;
    while (pBantu != NULL)
    {
        if (pBantu->nomor_pasien == key)
        {
            return pBantu;
        }
        pBantu = pBantu->next;
    }
    return NULL;
}

int main()
{
    pointer a, pCari;
    int key;
    int pil;
    list pasien;

    createList(pasien);
    do
    {
        system("cls");
        cout << "========================================================================" << endl;
        cout << "                    Program Daftar Pasien Well Puskesmas                " << endl;
        cout << "========================================================================" << endl;
        cout << "1. Insert First" << endl;
        cout << "2. Insert Last" << endl;
        cout << "3. Delete First" << endl;
        cout << "4. Delete Last" << endl;
        cout << "5. Traversal" << endl;
        cout << "6. Searching" << endl;
        cout << "7. Keluar" << endl;
        cout << "Masukkan Pilihan : ";
        cin >> pil;

        switch (pil)
        {
        case 1:
            createElmt(a);
            cout << endl;
            InsertFirst(pasien, a);
            system("pause");
            break;
        case 2:
            createElmt(a);
            cout << endl;
            InsertLast(pasien, a);
            system("pause");
            break;
        case 3:
            DeleteFirst(pasien, a);
            system("pause");
            break;
        case 4:
            DeleteLast(pasien, a);
            system("pause");
            break;
        case 5:
            traversal(pasien);
            system("pause");
            break;
        case 6:
            cout << "Masukkan nomor pasien yang ingin dicari: ";
            cin >> key;
            pCari = search(pasien, key);
            if (pCari != NULL)
            {
                cout << "Data ditemukan:" << endl;
                cout << "\nNomor Pasien          : " << pCari->nomor_pasien << endl;
                cout << "Nama Pasien             : " << pCari->nama_pasien << endl;
                cout << "Tanggal Lahir Pasien    : " << pCari->tgllahir << endl;
            }
            else
            {
                cout << "Data tidak ditemukan." << endl;
            }
            system("pause");
            break;
        case 7:
            cout << "Anda telah keluar dari program." << endl;
            break;
        default:
            cout << endl;
            cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
            system("pause");
            break;
        }
    } while (pil != 9);

    return 0;
}
