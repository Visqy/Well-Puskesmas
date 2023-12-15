#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <conio.h>
#include <ctime>
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
            cout << "Silakan Tunggu" << endl;
            handlePasienQueue();
        }
    }

    void handleSuster()
    {
        string option;
        do
        {
            cin.ignore();
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
        string option;
        do
        {
            cout << "\n1. Panggil pasien selanjutnya\n";
            cout << "2. Exit\n";
            cout << "Choose option: ";
            cin >> option;
            if (option == "1")
            {
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
                //
                // PR : disini tampilkan informasi pasien
                //
                Diagnosa diagnosaPasien = new diagnosa;
                cout << "Masukkan hasil diagnosa : ";
                getline(cin, diagnosaPasien->hasilDiagnosa);
                time_t now = time(0);
                tm *localTime = localtime(&now);
                char bufferTime[80];
                strftime(bufferTime, sizeof(bufferTime), "%Y-%m-%d %H:%M:%S", localTime);
                string waktu_sekarang(bufferTime);
                diagnosaPasien->waktuDiagnosa = waktu_sekarang;
                write(socket_, buffer("ADD_DIAGNOSA#" + diagnosaPasien->hasilDiagnosa + "#" + diagnosaPasien->waktuDiagnosa + "\n"));
                string pilihan;
                do
                {
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
        string option;
        do
        {
            cout << "\n1. Panggil pasien selanjutnya\n";
            cout << "2. Exit\n";
            cout << "Choose option: ";
            cin >> option;
            if (option == "1")
            {
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
                //
                // PR : tampilkan data pasien dan treatment
                //
                long total = 0, totalDibayarkan;
                time_t now = time(0);
                tm *localTime = localtime(&now);
                char bufferTime[80];
                strftime(bufferTime, sizeof(bufferTime), "%Y-%m-%d %H:%M:%S", localTime);
                string waktu_sekarang(bufferTime);
                cout << "Masukkan jumlah yang dibayarkan : ";
                cin >> totalDibayarkan;
                write(socket_, buffer("ADD_TRANSAKSI#" + to_string(total) + "#" + to_string(totalDibayarkan) + "#" + waktu_sekarang + "\n"));
                //
                // PR : traversal struk
                //
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
        string option;
        do
        {
            // header
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
                        } while (isError);
                    }
                    else if (option == "2")
                    {
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
                        bool isError;
                        string username;
                        write(socket_, buffer("DELETE_USER\n"));
                        do
                        {
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
                        // PR
                        write(socket_, buffer("TRAVERSAL_USER\n"));
                    }
                } while (option != "5");
                option = "";
            }
            else if (option == "2")
            {
                do
                {
                    // header
                    cout << "1. Perbarui Data Pribadi Pasien \n";
                    cout << "2. Hapus Pasien\n";
                    cout << "3. Tampilkan Seluruh Pasien\n";
                    cout << "4. Kembali\n";
                    cout << "Masukkan pilihan: ";
                    cin >> option;
                    if (option == "1")
                    {
                        bool isError;
                        int nomor;
                        string nama, alamat, tempatLahir, tanggalLahir, kontak, jenisKelamin;
                        do
                        {
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
                        } while (isError);
                    }
                    else if (option == "2")
                    {
                        bool isError;
                        int nomor;
                        write(socket_, buffer("DELETE_PASIEN\n"));
                        do
                        {
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
                        // pr
                    }
                } while (option != "4");
                option = "";
            }
            else if (option == "3")
            {
                do
                {
                    // header
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
                        bool isError;
                        string nama, username, password, poliklinik;
                        write(socket_, buffer("ADD_NEW_DOKTER\n"));
                        do
                        {
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
                        } while (isError);
                    }
                    else if (option == "2")
                    {
                        bool isError;
                        string newNama, newUsername, newPassword, newPoliklinik;
                        write(socket_, buffer("UPDATE_DOKTER\n"));
                        do
                        {
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
                        } while (isError);
                    }
                    else if (option == "3")
                    {
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
                        // PR
                    }
                } while (option != "5");
                option = "";
            }
            else if (option == "4")
            {
                // PR
                write(socket_, buffer("ALL_RIWAYAT_TRANSAKSI\n"));
            }
            else if (option == "6")
            {
                cout << "Server dihentikan ..." << endl;
                write(socket_, buffer("SHUTDOWN_SERVER\n"));
                exit(0);
            }
        } while (option != "5");
    }

private:
    void handlePasienQueue()
    {
        // Receive and display the message from the server
        read_until(socket_, dynamicBuffer, '\n');
        string callStatus;
        istream is(&dynamicBuffer);
        getline(is, callStatus);

        cout << "Server response: " << callStatus;
        if (callStatus.find("Anda dipanggil!") != string::npos)
        {
            cout << "\nTekan sembarang keyboard dalam waktu 5 detik untuk melanjutkan ..." << endl;
            char key;
            bool checkKey = waitForAnyKey(key, 5);
            if (checkKey)
            {
                write(socket_, buffer("OKE\n"));
                cout << "Silakan lanjut ke bagian Pendaftaran" << endl;
                handlePendaftaran();
                cout << "\nSilakan lanjut ke bagian Pemeriksaan Awal" << endl;
                int c;
                //
                // PR TAMPILKAN HASIL PEMERIKSAAN
                //
                string callStatusArray[2];
                read_until(socket_, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, callStatus);
                splitString(callStatus, "-", callStatusArray, c);
                if (callStatusArray[0].find("PEMERIKSAAN_AWAL_DONE") != string::npos)
                {
                    cout << "Proses pemeriksaan awal selesai." << endl;
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
            do
            {
                isError = false;
                write(socket_, buffer("NEW_PASIEN\n"));
                cout << "Masukkan nomor pasien: ";
                cin >> nomorPasien_;
                string regisStatus;
                write(socket_, buffer(to_string(nomorPasien_) + "\n"));
                read_until(socket_, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, regisStatus);
                if (regisStatus.find("PASIEN_FOUND") != string::npos)
                {
                    cout << "Nomor pasien sudah ada" << endl;
                    isError = true;
                }
                else
                {
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
                }
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
