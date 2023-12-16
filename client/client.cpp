#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <iomanip>
#include <sstream>
#include "structClient.cpp"
#include "etc.cpp"
// #include "userManagement.cpp"
// #include "dokterManagement.cpp"

using namespace std;
using namespace boost::asio;

io_service ioService;

class TCPClient
{
public:
    TCPClient(io_service &ioService, const string &serverAddress, short port)
        : socket_(ioService), resolver_(ioService), serverAddress_(serverAddress), serverPort_(port)
    {
        // Resolve the server address
        ip::tcp::resolver::results_type endpoints = resolver_.resolve(serverAddress, to_string(port));

        // Connect to the server
        connect(socket_, endpoints);
    }

    bool login(const string &username, const string &password, UserType &userType)
    {
        // Send username and password to the server
        write(socket_, buffer(username + '\n'));
        write(socket_, buffer(password + '\n'));

        // Receive and display login status message from the server
        string loginStatus;
        read_until(socket_, dynamicBuffer, '\n');
        istream is(&dynamicBuffer);
        getline(is, loginStatus);
        const int maxSize = 100;
        string result[maxSize];
        string delimiter = "-";
        int count;
        splitString(loginStatus, delimiter, result, count);

        cout << "Server response: " << result[0];

        // Check if login is successful
        if (result[0].find("Login Successful") != string::npos)
        {
            // Set user type
            cout << "\nServer response: Login sebagai " << result[1];
            userType = convertStringToUserType(result[1]);
            return true;
        }
        else
        {
            // If login fails, close the current socket
            socket_.close();

            // Create a new socket and connect to the server
            ip::tcp::resolver resolver(ioService_);
            ip::tcp::resolver::results_type endpoints = resolver.resolve(serverAddress_, to_string(serverPort_));
            socket_ = ip::tcp::socket(ioService_);
            connect(socket_, endpoints);
            return false;
        }
    }

    void handlePatient()
    {
        auto printHeader = [](const string &title)
        {
            cout << "=====================================\n";
            cout << "            " << title << "\n";
            cout << "=====================================\n";
        };
        system("cls");
        cin.ignore();
        printHeader("Menu Pasien (Online)");
        string patientName, pilihPoliklinik;
        bool isError, checkPoliklinikOpen;
        cout << "\nMasukkan nama antrian pasien: ";
        getline(cin, patientName);
        do
        {
            isError = false;
            cout << "Pilih poliklinik : ";
            cin >> pilihPoliklinik;
            if (convertStringToPoliklinik(pilihPoliklinik) != AFK)
            {
                string poliklinikStatus;
                write(socket_, buffer("CHECK_POLIKLINIK-" + pilihPoliklinik + "\n"));
                read_until(socket_, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, poliklinikStatus);
                if (poliklinikStatus.find("POLIKLINIK_CLOSED") != string::npos)
                {
                    cout << "Poliklinik yang dipilih sedang tutup" << endl;
                    isError = true;
                }
            }
            else
            {
                cout << "Poliklinik tidak valid, Silakan pilih poliklinik kembali" << endl;
                isError = true;
            }
        } while (isError);
        // Register to the queue
        write(socket_, buffer("REGISTER-" + patientName + "-" + pilihPoliklinik + "\n"));
        // Receive registration response
        string registrationStatus;
        read_until(socket_, dynamicBuffer, '\n');
        istream is(&dynamicBuffer);
        getline(is, registrationStatus);
        if (registrationStatus.find("Registration Successful") != string::npos)
        {
            // Wait for call
            cout << "\nSilakan Tunggu" << endl;
            handlePasienQueue();
        }
    }

    void handleSuster()
    {
        string option;
        auto printHeader = [](const string &title)
        {
            cout << "=====================================\n";
            cout << "            " << title << "\n";
            cout << "=====================================\n";
        };
        do
        {
            system("cls");
            cin.ignore();
            printHeader("Menu Suster");
            cout << "\n1. Panggil pasien selanjutnya\n";
            cout << "2. Tambahkan antrian\n";
            cout << "3. Exit\n";
            cout << "Choose option: ";
            cin >> option;

            if (option == "1")
            {
                // Dequeue
                write(socket_, buffer("DEQUEUE\n"));

                // Receive dequeue response
                int count;
                string dequeueStatus, dequeueStatusArray[3];
                read_until(socket_, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, dequeueStatus);
                splitString(dequeueStatus, "-", dequeueStatusArray, count);
                cout << dequeueStatusArray[0] << endl;
                if (dequeueStatus.find("Pemanggilan pasien") != string::npos)
                {
                    cout << "Atas nama " + dequeueStatusArray[1] << endl;
                    if (dequeueStatusArray[2] == "false")
                    {
                        handlePendaftaran();
                        handlePemeriksaanAwal();
                    }
                    else
                    {
                        int count;
                        string regisStatus;
                        cout << "Silakan tunggu pasien (online) mengisi pendaftaran ..." << endl;
                        read_until(socket_, dynamicBuffer, '\n');
                        istream is(&dynamicBuffer);
                        getline(is, regisStatus);
                        if (regisStatus.find("REGIS_DONE") != string::npos)
                        {
                            cout << "Pendaftaran pasien (online) selesai" << endl;
                            handlePemeriksaanAwal();
                        }
                    }
                }
            }
            else if (option == "2")
            {
                write(socket_, buffer("REGISTER_OFFLINE\n"));
                string patientName, pilihPoliklinik;
                bool isError, checkPoliklinikOpen;
                cout << "\nMasukkan nama antrian pasien: ";
                getline(cin, patientName);
                do
                {
                    cin.ignore();
                    isError = false;
                    cout << "Pilih poliklinik:";
                    getline(cin, pilihPoliklinik);
                    if (convertStringToPoliklinik(pilihPoliklinik) != AFK)
                    {
                        string poliklinikStatus;
                        write(socket_, buffer("CHECK_POLIKLINIK-" + pilihPoliklinik + "\n"));
                        read_until(socket_, dynamicBuffer, '\n');
                        istream is(&dynamicBuffer);
                        getline(is, poliklinikStatus);
                        if (poliklinikStatus.find("POLIKLINIK_CLOSED") != string::npos)
                        {
                            cout << "Poliklinik yang dipilih sedang tutup" << endl;
                            isError = true;
                        }
                    }
                    else
                    {
                        cout << "Poliklinik tidak valid, Silakan pilih poliklinik kembali" << endl;
                        isError = true;
                    }
                } while (isError);
                // Register to the queue
                write(socket_, buffer("REGISTER-" + patientName + "-" + pilihPoliklinik + "\n"));
                // Receive registration response
                string registrationStatus;
                read_until(socket_, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, registrationStatus);
                cout << "Server response: " << registrationStatus;
                cout << "\nTambah pasien berhasil" << endl;
            }
            else if (option != "3")
            {
                cout << "Invalid option. Try again.\n";
            }
            cout << endl;
            system("pause");
        } while (option != "3");
    }

    void handleDokter()
    {
        auto printHeader = [](const string &title)
        {
            cout << "=====================================\n";
            cout << "            " << title << "\n";
            cout << "=====================================\n";
        };

        string option, optionArray[2];
        read_until(socket_, dynamicBuffer, "\n");
        string response;
        int count;
        istream is(&dynamicBuffer);
        getline(is, response);
        splitString(response, "#", optionArray, count);
        do
        {
            system("cls");
            cin.ignore();
            printHeader("Menu Dokter");
            cout << "Nama Dokter : " << optionArray[0] << endl;
            cout << "Poliklinik : " << optionArray[1] << endl;
            cout << "=====================================\n";

            cout << "\n1. Panggil pasien selanjutnya\n";
            cout << "2. Exit\n";
            cout << "Masukkan pilihan: ";
            cin >> option;
            if (option == "1")
            {
                system("cls");
                printHeader("Menu Dokter");

                write(socket_, buffer("NEXT_PASIEN\n"));
                read_until(socket_, dynamicBuffer, "\n");
                is.clear();
                getline(is, response);
                if (response.find("EMPTY_QUEUE") != string::npos)
                {
                    cout << "Antrian kosong. Mohon ditunggu." << endl;
                    system("PAUSE");
                    continue;
                }

                string dataPribadiString, pemeriksaanAwalString;
                read_until(socket_, dynamicBuffer, "\n");
                is.clear();
                getline(is, dataPribadiString);
                read_until(socket_, dynamicBuffer, "\n");
                is.clear();
                getline(is, pemeriksaanAwalString);
                printDataPribadi(createDataPribadiFromString(dataPribadiString));
                displayOutputPemeriksaan(createPemeriksaanAwalFromString(pemeriksaanAwalString));

                Diagnosa diagnosaPasien = new diagnosa;
                cin.ignore();
                cout << "Masukkan hasil diagnosa : ";
                getline(cin, diagnosaPasien->hasilDiagnosa);
                write(socket_, buffer("ADD_DIAGNOSA#" + diagnosaPasien->hasilDiagnosa + "\n"));
                string pilihan;
                do
                {
                    cin.ignore();
                    Treatment pBantu = new treatment;
                    cout << "Masukkan treatment yang dilakukan : ";
                    getline(cin, pBantu->namaTreatment);
                    cout << "Masukkan harga treatment : ";
                    cin >> pBantu->biaya;
                    string message = "ADD_TREATMENT#" + pBantu->namaTreatment + "#" + to_string(pBantu->biaya) + "\n";
                    write(socket_, buffer(message));
                    cout << "Tambah treatment (Y/N) ";
                    cin >> pilihan;
                    if (pilihan == "N" || pilihan == "n")
                    {
                        write(socket_, buffer("STOP_TREATMENT#\n"));
                        delete pBantu;
                        break;
                    }
                } while (pilihan != "N" || pilihan != "n");
            }
            else if (option != "2")
            {
                socket_.close();
                break;
            }
        } while (option != "2");
    }

    void handleKasir()
    {
        auto printHeader = [](const string &title)
        {
            cout << "=====================================\n";
            cout << "            " << title << "\n";
            cout << "=====================================\n";
        };

        string option;
        do
        {
            system("cls");
            cin.ignore();
            printHeader("Menu Kasir");
            cout << "\n1. Panggil pasien selanjutnya\n";
            cout << "2. Exit\n";
            cout << "Choose option: ";
            cin >> option;
            if (option == "1")
            {
                system("cls");
                cin.ignore();
                printHeader("Menu Kasir");
                write(socket_, buffer("NEXT_PASIEN\n"));
                read_until(socket_, dynamicBuffer, "\n");
                string response;
                istream is(&dynamicBuffer);
                getline(is, response);
                if (response.find("EMPTY_QUEUE") != string::npos)
                {
                    cout << "Antrian kosong. Mohon ditunggu." << endl;
                    continue;
                }

                string dataPribadiString, diagnosaString, etcString, etcStringArray[3];
                riwayatTransaksi rt;
                int count;
                read_until(socket_, dynamicBuffer, "\n");
                is.clear();
                getline(is, dataPribadiString);
                read_until(socket_, dynamicBuffer, "\n");
                is.clear();
                getline(is, diagnosaString);
                read_until(socket_, dynamicBuffer, "\n");
                is.clear();
                getline(is, etcString);
                splitString(etcString, "#", etcStringArray, count);
                // Diagnosa hasilDiagnosa = createDiagnosaFromString(diagnosaString);
                // cout << hasilDiagnosa->id << endl;
                printDataPribadi(createDataPribadiFromString(dataPribadiString));
                // printTreatment(hasilDiagnosa->treatmentDiagnosa);

                rt.idPasien = extractNumber(etcStringArray[0]);
                // rt.treatmentPasien = hasilDiagnosa->treatmentDiagnosa;

                cout << "Masukkan jumlah yang dibayarkan : ";
                cin >> rt.jumlahDibayarkan;

                write(socket_, buffer("ADD_TRANSAKSI#" + to_string(rt.total) + "#" + to_string(rt.jumlahDibayarkan) + "\n"));
                system("PAUSE");
                system("CLS");
                cetakNota(rt, etcStringArray[2], etcStringArray[1]);
                system("PAUSE");
            }
            else if (option != "2")
            {
                socket_.close();
                break;
            }
        } while (option != "2");
    };

    void handleAdmin()
    {
        auto printHeader = [](const string &title)
        {
            cout << "=====================================\n";
            cout << "            " << title << "\n";
            cout << "=====================================\n";
        };

        string option;
        do
        {
            system("cls");
            printHeader("Menu Admin");
            cin.ignore();
            cout << "\n1. Manajemen User \n";
            cout << "2. Manajemen Data Pasien\n";
            cout << "3. Manajemen Dokter\n";
            cout << "4. Tampilkan Riwayat Transaksi\n";
            cout << "5. Exit\n";
            cout << "6. Hentikan Server\n";
            cout << "Masukkan pilihan: ";
            cin >> option;
            if (option == "1")
            {
                // header
                do
                {
                    system("cls");
                    printHeader("Menu Manajemen User");
                    cin.ignore();
                    option = "";
                    cout << "\n1. Tambahkan User \n";
                    cout << "2. Perbarui User \n";
                    cout << "3. Hapus User\n";
                    cout << "4. Tampilkan Seluruh User\n";
                    cout << "5. Kembali\n";
                    cout << "Masukkan pilihan: ";
                    cin >> option;
                    if (option == "1")
                    {
                        system("cls");
                        printHeader("Menu Manajemen User");
                        cin.ignore();
                        bool isError;
                        UserType usertype;
                        string username, password;
                        write(socket_, buffer("ADD_NEW_USER\n"));
                        do
                        {
                            isError = false;
                            cout << "Masukkan username: ";
                            cin >> username;
                            cout << "Masukkan password: ";
                            cin >> password;
                            string type;
                            cout << "Masukkan tipe user: ";
                            cin >> type;
                            usertype = convertStringToUserType(type);
                            if (usertype == PASIEN || usertype == DOKTER)
                            {
                                cout << "Tipe user tidak valid" << endl;
                                isError = true;
                                write(socket_, buffer("USER_FAIL\n"));
                                continue;
                            }
                            write(socket_, buffer(username + "-" + password + "-" + type + "\n"));
                            // Respose from server
                            string userStatus;
                            read_until(socket_, dynamicBuffer, '\n');
                            istream is(&dynamicBuffer);
                            getline(is, userStatus);
                            if (userStatus.find("USER_FAIL") != string::npos)
                            {
                                cout << "User sudah ada" << endl;
                                isError = true;
                                continue;
                            }
                            else
                            {
                                cout << "User berhasil dibuat" << endl;
                            }
                            cin.ignore();
                        } while (isError);
                    }
                    else if (option == "2")
                    {
                        system("cls");
                        printHeader("Menu Manajemen User");
                        cin.ignore();
                        bool isError;
                        string username, password, newPassword, newUserType;
                        UserType userType;
                        write(socket_, buffer("UPDATE_USER\n"));
                        do
                        {
                            isError = false;
                            cout << "Masukkan username yang akan diperbarui : ";
                            cin >> username;
                            write(socket_, buffer(username + "\n"));
                            // Respose from server
                            string userStatus;
                            read_until(socket_, dynamicBuffer, '\n');
                            istream is(&dynamicBuffer);
                            getline(is, userStatus);
                            if (userStatus.find("USER_FOUND") != string::npos)
                            {
                                cin.ignore();
                                cout << "Masukkan password baru : ";
                                cin >> newPassword;
                                cout << "Masukkan tipe user : ";
                                cin >> newUserType;
                                userType = convertStringToUserType(newUserType);
                                if (userType == PASIEN || userType == DOKTER)
                                {
                                    cout << "Tipe user tidak valid" << endl;
                                    isError = true;
                                    continue;
                                }
                                write(socket_, buffer(newPassword + "-" + newUserType + "\n"));
                                // Respose from server
                                string userStatus;
                                read_until(socket_, dynamicBuffer, '\n');
                                istream is(&dynamicBuffer);
                                getline(is, userStatus);
                                if (userStatus.find("USER_UPDATED") != string::npos)
                                {
                                    cout << "User berhasil diperbarui" << endl;
                                }
                                else
                                {
                                    cout << "User gagal diperbarui" << endl;
                                    isError = true;
                                }
                            }
                            else if (userStatus.find("USER_CANT_CHANGE") != string::npos)
                            {
                                cout << "User yang dipilih merupakan dokter/pasien, tidak dapat merubah." << endl;
                                isError = true;
                            }
                            else
                            {
                                cout << "User tidak ditemukan" << endl;
                                isError = true;
                            }
                        } while (isError);
                    }
                    else if (option == "3")
                    {
                        system("cls");
                        printHeader("Menu Manajemen User");
                        cin.ignore();
                        bool isError;
                        string username;
                        write(socket_, buffer("DELETE_USER\n"));
                        do
                        {
                            cin.ignore();
                            cout << "Masukkan username yang akan dihapus :";
                            cin >> username;
                            write(socket_, buffer(username + "\n"));
                            // Respose from server
                            string userStatus;
                            read_until(socket_, dynamicBuffer, '\n');
                            istream is(&dynamicBuffer);
                            getline(is, userStatus);
                            if (userStatus.find("USER_FOUND") != string::npos)
                            {
                                cout << "User berhasil dihapus" << endl;
                            }
                            else
                            {
                                cout << "User tidak ditemukan" << endl;
                                isError = true;
                            }
                        } while (isError);
                    }
                    else if (option == "4")
                    {
                        system("cls");
                        printHeader("Menu Manajemen User");
                        cin.ignore();
                        write(socket_, buffer("TRAVERSAL_USER\n"));
                    }
                    system("PAUSE");
                } while (option != "5");
                option = "";
            }
            else if (option == "2")
            {
                do
                {
                    system("cls");
                    printHeader("Menu Manajemen Pasien");
                    cin.ignore();
                    cout << "1. Perbarui Data Pribadi Pasien \n";
                    cout << "2. Hapus Pasien\n";
                    cout << "3. Tampilkan Seluruh Pasien\n";
                    cout << "4. Kembali\n";
                    cout << "Masukkan pilihan: ";
                    cin >> option;
                    if (option == "1")
                    {
                        system("cls");
                        printHeader("Menu Manajemen Pasien");
                        cin.ignore();
                        bool isError;
                        int nomor;
                        string nama, alamat, tempatLahir, tanggalLahir, kontak, jenisKelamin;
                        do
                        {
                            cin.ignore();
                            isError = false;
                            cout << "Masukkan nomor pasien yang akan diperbarui : ";
                            cin >> nomor;
                            write(socket_, buffer(to_string(nomor) + "\n"));
                            // Respose from server
                            string pasienStatus;
                            read_until(socket_, dynamicBuffer, '\n');
                            istream is(&dynamicBuffer);
                            getline(is, pasienStatus);
                            if (pasienStatus.find("PASIEN_FOUND") != string::npos)
                            {
                                cin.ignore();
                                cout << "Masukkan nama pasien :";
                                getline(cin, nama);
                                cout << "Masukkan alamat pasien :";
                                getline(cin, alamat);
                                cout << "Masukkan tempat lahir pasien :";
                                getline(cin, tempatLahir);
                                cout << "Masukkan tanggal lahir pasien :";
                                getline(cin, tanggalLahir);
                                cout << "Masukkan jenis kelamin pasien :";
                                getline(cin, jenisKelamin);
                                cout << "Masukkan nomor kontak pasien :";
                                cin >> kontak;
                                write(socket_, buffer(nama + "-" + alamat + "-" + tempatLahir + "-" + tanggalLahir + "-" + kontak + "-" + jenisKelamin + "-" + "\n"));
                                // Respose from server
                                string userStatus;
                                read_until(socket_, dynamicBuffer, '\n');
                                istream is(&dynamicBuffer);
                                getline(is, userStatus);
                                if (userStatus.find("PASIEN_UPDATED") != string::npos)
                                {
                                    cout << "Data pribadi pasien berhasil diperbarui" << endl;
                                }
                                else
                                {
                                    cout << "Data pribadi pasien gagal diperbarui" << endl;
                                    isError = true;
                                }
                            }
                            else
                            {
                                cout << "Pasien tidak ditemukan" << endl;
                                isError = true;
                            }
                            system("Pause");
                        } while (isError);
                    }
                    else if (option == "2")
                    {
                        system("cls");
                        printHeader("Menu Manajemen Pasien");
                        cin.ignore();
                        bool isError;
                        int nomor;
                        write(socket_, buffer("DELETE_PASIEN\n"));
                        do
                        {
                            cin.ignore();
                            cout << "Masukkan nomor pasien yang akan dihapus : ";
                            cin >> nomor;
                            write(socket_, buffer(to_string(nomor) + "\n"));
                            // Respose from server
                            string pasienStatus;
                            read_until(socket_, dynamicBuffer, '\n');
                            istream is(&dynamicBuffer);
                            getline(is, pasienStatus);
                            if (pasienStatus.find("PASIEN_FOUND") != string::npos)
                            {
                                cout << "Pasien berhasil dihapus" << endl;
                            }
                            else
                            {
                                cout << "Pasien tidak ditemukan" << endl;
                                isError = true;
                            }
                        } while (isError);
                    }
                    else if (option == "3")
                    {
                        system("cls");
                        printHeader("Menu Manajemen Pasien");
                        cin.ignore();
                        // pr
                    }
                    system("PAUSE");
                } while (option != "4");
                option = "";
            }
            else if (option == "3")
            {
                do
                {
                    system("cls");
                    printHeader("Menu Manajemen Dokter");
                    cin.ignore();
                    option = "";
                    cout << "\n1. Tambahkan Dokter \n";
                    cout << "2. Perbarui Dokter \n";
                    cout << "3. Hapus Dokter\n";
                    cout << "4. Tampilkan Seluruh Dokter\n";
                    cout << "5. Kembali\n";
                    cout << "Masukkan pilihan: ";
                    cin >> option;
                    if (option == "1")
                    {
                        system("cls");
                        printHeader("Menu Manajemen Dokter");
                        cin.ignore();
                        bool isError;
                        string nama, username, password, poliklinik;
                        write(socket_, buffer("ADD_NEW_DOKTER\n"));
                        do
                        {
                            cout << "Tambah Data Dokter" << endl;
                            isError = false;
                            cout << "Masukkan nama : ";
                            getline(cin, nama);
                            cout << "Masukkan poliklinik : ";
                            cin >> poliklinik;
                            cout << "Masukkan username untuk login : ";
                            cin >> username;
                            cout << "Masukkan password untuk login : ";
                            cin >> password;
                            write(socket_, buffer(nama + "-" + poliklinik + "-" + username + "-" + password + "\n"));
                            // Respose from server
                            string userStatus;
                            read_until(socket_, dynamicBuffer, '\n');
                            istream is(&dynamicBuffer);
                            getline(is, userStatus);
                            if (userStatus.find("DOKTER_FAIL") != string::npos)
                            {
                                cout << "Dokter sudah ada" << endl;
                                isError = true;
                                continue;
                            }
                            else
                            {
                                cout << "Dokter berhasil ditambahkan" << endl;
                            }
                            cin.ignore();
                        } while (isError);
                    }
                    else if (option == "2")
                    {
                        system("cls");
                        printHeader("Menu Manajemen Dokter");
                        cin.ignore();
                        bool isError;
                        string newNama, newUsername, newPassword, newPoliklinik;
                        write(socket_, buffer("UPDATE_DOKTER\n"));
                        do
                        {
                            cout << "Update Data Dokter" << endl;
                            isError = false;
                            cout << "Masukkan nama dokter yang akan diperbarui : ";
                            cin >> newNama;
                            write(socket_, buffer(newNama + "\n"));
                            // Respose from server
                            string userStatus;
                            read_until(socket_, dynamicBuffer, '\n');
                            istream is(&dynamicBuffer);
                            getline(is, userStatus);
                            if (userStatus.find("DOKTER_FOUND") != string::npos)
                            {
                                cout << "Masukkan poliklinik baru : ";
                                cin >> newPoliklinik;
                                cout << "Masukkan username baru : ";
                                cin >> newUsername;
                                cout << "Masukkan password baru : ";
                                cin >> newPassword;
                                write(socket_, buffer(newPoliklinik + "-" + newUsername + "-" + newPassword + "\n"));
                                // Respose from server
                                string userStatus;
                                read_until(socket_, dynamicBuffer, '\n');
                                istream is(&dynamicBuffer);
                                getline(is, userStatus);
                                if (userStatus.find("DOKTER_UPDATED") != string::npos)
                                {
                                    cout << "Dokter berhasil diperbarui" << endl;
                                }
                                else
                                {
                                    cout << "Dokter gagal diperbarui" << endl;
                                    isError = true;
                                }
                            }
                            cin.ignore();
                        } while (isError);
                    }
                    else if (option == "3")
                    {
                        system("cls");
                        printHeader("Menu Manajemen Dokter");
                        cin.ignore();
                        bool isError;
                        int id;
                        write(socket_, buffer("DELETE_DOKTER\n"));
                        do
                        {
                            cout << "Masukkan id dokter yang akan dihapus :";
                            cin >> id;
                            write(socket_, buffer(to_string(id) + "\n"));
                            // Respose from server
                            string userStatus;
                            read_until(socket_, dynamicBuffer, '\n');
                            istream is(&dynamicBuffer);
                            getline(is, userStatus);
                            if (userStatus.find("DOKTER_FOUND") != string::npos)
                            {
                                cout << "Dokter berhasil dihapus" << endl;
                            }
                            else
                            {
                                cout << "Dokter tidak ditemukan" << endl;
                                isError = true;
                            }
                        } while (isError);
                    }
                    else if (option == "4")
                    {
                        system("cls");
                        printHeader("Menu Manajemen Dokter");
                        cin.ignore();
                        // PR
                    }
                    system("PAUSE");
                } while (option != "5");
                option = "";
            }
            else if (option == "4")
            {
                system("cls");
                printHeader("Menu Lihat Riwayat Transaksi");
                cin.ignore();
                // PR
                write(socket_, buffer("ALL_RIWAYAT_TRANSAKSI\n"));
            }
            else if (option == "6")
            {
                system("cls");
                printHeader("Menu Manajemen Server");
                cin.ignore();
                cout << "Server dihentikan ..." << endl;
                write(socket_, buffer("SHUTDOWN_SERVER\n"));
                exit(0);
            }
            system("PAUSE");
        } while (option != "5");
    }

private:
    void handlePasienQueue()
    {
        // Receive and display the message from the server
        auto printHeader = [](const string &title)
        {
            cout << "=====================================\n";
            cout << "            " << title << "\n";
            cout << "=====================================\n";
        };

        read_until(socket_, dynamicBuffer, '\n');
        string callStatus;
        istream is(&dynamicBuffer);
        getline(is, callStatus);
        if (callStatus.find("Anda dipanggil!") != string::npos)
        {
            system("cls");
            printHeader("Menu Pasien (Online)");

            cout << endl
                 << callStatus << endl;
            cout << "\nTekan sembarang keyboard dalam waktu 5 detik untuk melanjutkan ..." << endl;
            cin.ignore();
            char key;
            bool checkKey = waitForAnyKey(key, 5);
            if (checkKey)
            {
                write(socket_, buffer("OKE\n"));
                cout << "Silakan lanjut ke bagian Pendaftaran" << endl;
                handlePendaftaran();
                cout << "\nSilakan lanjut ke bagian Pemeriksaan Awal" << endl;
                int c;

                string callStatusArray[2];
                read_until(socket_, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, callStatus);
                splitString(callStatus, "-", callStatusArray, c);
                if (callStatusArray[0].find("PEMERIKSAAN_AWAL_DONE") != string::npos)
                {
                    cout << "Proses pemeriksaan awal selesai." << endl;
                    string pemeriksaanAwalString;
                    read_until(socket_, dynamicBuffer, '\n');
                    is.clear();
                    getline(is, pemeriksaanAwalString);
                    pemeriksaanAwal pA = createPemeriksaanAwalFromString(pemeriksaanAwalString);
                    displayOutputPemeriksaan(pA);
                    printHeader("Menu Pasien (Online)");
                    cout << "Silakan lanjut ke Poliklinik " + callStatusArray[1] << endl;
                    cout << "Silakan Tunggu ..." << endl;
                    read_until(socket_, dynamicBuffer, '\n');
                    istream is(&dynamicBuffer);
                    getline(is, callStatus);
                    if (callStatus.find("CALL_PASIEN_POLIKLINIK") != string::npos)
                    {
                        cout << "Anda dipanggil!" << endl;
                        read_until(socket_, dynamicBuffer, '\n');
                        istream is(&dynamicBuffer);
                        getline(is, callStatus);
                        if (callStatus.find("PASIEN_POLIKLINIK_DONE") != string::npos)
                        {
                            //
                            // PR TAMPILKAN HASIL DIAGNOSA
                            //
                            cout << "Proses poliklik selesai." << endl;
                            cout << "Silakan lanjut ke Kasir " << endl;
                            cout << "Silakan Tunggu ..." << endl;

                            read_until(socket_, dynamicBuffer, '\n');
                            istream is(&dynamicBuffer);
                            getline(is, callStatus);
                            if (callStatus.find("CALL_PASIEN_KASIR") != string::npos)
                            {
                                cout << "Anda dipanggil!" << endl;
                                read_until(socket_, dynamicBuffer, '\n');
                                istream is(&dynamicBuffer);
                                getline(is, callStatus);
                                if (callStatus.find("PASIEN_KASIR_DONE") != string::npos)
                                {
                                    //
                                    // PR TAMPILKAN HASIL STRUK
                                    //
                                    cout << "Proses kasir selesai." << endl;
                                    cout << "Terimakasih" << endl;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                write(socket_, buffer("RESET\n"));
                cout << "Anda tidak merespon silakan tunggu kembali" << endl;
                handlePasienQueue();
            }
        }
    }

    void handlePendaftaran()
    {
        bool isError;
        dataPribadi pBantu;
        string pilihan;
        // header
        cout << "\nApakah anda pasien baru? (Y/N)\n";
        cin >> pilihan;
        if (pilihan == "N" || pilihan == "n")
        {
            write(socket_, buffer("OLD_PASIEN\n"));
            do
            {
                cin.ignore();
                isError = false;
                cout << "Masukkan nomor pasien: ";
                cin >> nomorPasien_;
                int count;
                string regisStatus, regisStatusArray[2];
                write(socket_, buffer(to_string(nomorPasien_) + "\n"));
                read_until(socket_, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, regisStatus);
                splitString(regisStatus, "-", regisStatusArray, count);
                if (regisStatusArray[0].find("PASIEN_FOUND") != string::npos)
                {
                    read_until(socket_, dynamicBuffer, '\n');
                    istream is(&dynamicBuffer);
                    getline(is, regisStatus);
                    cout << "Selamat datang kembali, " + regisStatusArray[1] << endl;
                    // exit
                }
                else
                {
                    cout << "Pasien tidak ditemukan" << endl;
                    isError = true;
                }
            } while (isError);
        }
        else
        {
            string regisStatus;
            do
            {
                isError = false;
                write(socket_, buffer("NEW_PASIEN\n"));
                cin.ignore();
                cout << "Masukkan nama pasien baru : ";
                getline(cin, pBantu.nama);
                cout << "Masukkan alamat pasien baru : ";
                getline(cin, pBantu.alamat);
                cout << "Masukkan nomor telepon pasien baru :";
                getline(cin, pBantu.kontak);
                cout << "Masukkan tempat lahir pasien baru: ";
                getline(cin, pBantu.tempatLahir);
                cout << "Masukkan tanggal lahir pasien baru: ";
                getline(cin, pBantu.tanggalLahir);
                cout << "Masukkan jenis kelamin pasien baru: ";
                getline(cin, pBantu.jenisKelamin);
                write(socket_, buffer(pBantu.nama + "-" + pBantu.alamat + "-" + pBantu.kontak + "-" + pBantu.tempatLahir + "-" + pBantu.tanggalLahir + "-" + pBantu.jenisKelamin + "\n"));
                read_until(socket_, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, regisStatus);
                cout << "Server response : " << regisStatus;
            } while (isError);
        }
    }
    void handlePemeriksaanAwal()
    {
        cin.ignore();
        pemeriksaanAwal pAwal;
        cout << "Masukkan berat badan: ";
        cin >> pAwal.beratBadan;
        cout << "Masukkan tinggi badan: ";
        cin >> pAwal.tinggiBadan;
        cout << "Masukkan suhu badan: ";
        cin >> pAwal.suhuBadan;
        cout << "Masukkan tekanan darah" << endl;
        cout << "Sistolik :";
        cin >> pAwal.tekananDarah.sistolik;
        cout << "Diastolik :";
        cin >> pAwal.tekananDarah.diastolik;
        write(socket_, buffer(to_string(pAwal.beratBadan) + "-" + to_string(pAwal.tinggiBadan) + "-" + to_string(pAwal.suhuBadan) + "-" + to_string(pAwal.tekananDarah.sistolik) + "-" + to_string(pAwal.tekananDarah.diastolik) + "-" + "\n"));
        cout << "Data Pemeriksaan Awal berhasil ditambahkan" << endl;
    }

    Pasien pasien_;
    int nomorPasien_;
    io_service ioService_;
    ip::tcp::socket socket_;
    ip::tcp::resolver resolver_;
    string serverAddress_;
    short serverPort_;
    boost::asio::streambuf dynamicBuffer;
};

int main()
{
    TCPClient client(ioService, "127.0.0.1", 8080);
    system("CLS");
    bool loggedIn = false;
    cout << "Apakah anda pasien? (Y/N)";
    char choice;
    cin >> choice;
    if (choice == 'y' || choice == 'Y')
    {
        UserType userType;
        client.login("pasien", "pasien123", userType);
        client.handlePatient();
    }
    else
    {
        UserType userType;
        do
        {
            // Get user input for login
            string username, password;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;
            // Attempt login
            loggedIn = client.login(username, password, userType);

            if (!loggedIn)
            {
                cout << "Login failed. Try again.\n";
            }
        } while (!loggedIn);

        switch (userType)
        {
        case ADMIN:
            client.handleAdmin();
            break;
        case SUSTER:
            client.handleSuster();
            break;
        case DOKTER:
            client.handleDokter();
            break;
        case KASIR:
            client.handleKasir();
            break;
        }
    }

    ioService.run();

    return 0;
}
