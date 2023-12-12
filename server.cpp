// server.cpp
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <thread>
#include "passwordmanager.cpp"
#include "dokterFileManager.cpp"
#include "queue.cpp"
#include "struct.cpp"
#include "etc.cpp"

using namespace std;
using namespace boost::asio;

class HandleAdmin
{
public:
    HandleAdmin(ip::tcp::socket socket) : socket_(std::move(socket)) {}

    void startHandling()
    {
        try
        {
            while (true)
            {
                string Command;
                read_until(socket_, dynamicBuffer_, '\n');
                istream is(&dynamicBuffer_);
                getline(is, Command);
                cout << "dapat pesan " << Command << " dari admin" << endl;
                // User Manajemen
                if (Command.find("ADD_NEW_USER") != string::npos)
                {
                    addUser();
                }
                else if (Command.find("UPDATE_USER") != string::npos)
                {
                    updateUser();
                }
                else if (Command.find("DELETE_USER") != string::npos)
                {
                    deleteUser();
                }
                else if (Command.find("TRAVERSAL_USER") != string::npos)
                {
                    traversalUser();
                }
                // Pasien Manajemen
                else if (Command.find("UPDATE_PASIEN") != string::npos)
                {
                    updatePasien();
                }
                else if (Command.find("DELETE_PASIEN") != string::npos)
                {
                    deletePasien();
                }
                else if (Command.find("TRAVERSAL_PASIEN") != string::npos)
                {
                    traversalPasien();
                }
                // Riwayat Transaksi
                else if (Command.find("ALL_RIWAYAT_TRANSAKSI") != string::npos)
                {
                    allRiwayatTransaksi();
                }
                // Dokter Manajemen
                else if (Command.find("ADD_DOKTER") != string::npos)
                {
                    addDokter();
                }
                else if (Command.find("UPDATE_DOKTER") != string::npos)
                {
                    updateDokter();
                }
                else if (Command.find("DELETE_DOKTER") != string::npos)
                {
                    deleteDokter();
                }
                else if (Command.find("TRAVERSAL_DOKTER") != string::npos)
                {
                    traversalDokter();
                }
                // Server Manajemen
                else if (Command.find("SHUTDOWN_SERVER") != string::npos)
                {
                }
            }
        }
        catch (exception &e)
        {
            cerr << "Exception in thread: " << e.what() << endl;
        }
    }

private:
    void addUser()
    {
        while (true)
        {
            string userData, userArrayData[3];
            int count;
            UserType usertype;
            read_until(socket_, dynamicBuffer_, '\n');
            istream is(&dynamicBuffer_);
            getline(is, userData);
            splitString(userData, "-", userArrayData, count);
            if (checkUserIfExists(userArrayData[0], usertype))
            {
                write(socket_, buffer("USER_FAIL \n"));
                continue;
            }
            else
            {
                write(socket_, buffer("USER_SUCCESS \n"));
                saveUser(userArrayData[0], userArrayData[1], convertStringToUserType(userArrayData[2]));
                break;
            }
        }
    }
    void deleteUser()
    {
        while (true)
        {
            string username;
            int count;
            UserType usertype;
            read_until(socket_, dynamicBuffer_, '\n');
            istream is(&dynamicBuffer_);
            getline(is, username);
            if (checkUserIfExists(username, usertype))
            {
                deleteUserByUsername(username);
                write(socket_, buffer("USER_FOUND \n"));
                break;
            }
            else
            {
                write(socket_, buffer("USER_NOT_FOUND \n"));
                continue;
            }
        }
    }

    void updateUser()
    {
        while (true)
        {
            string userData, userArrayData[3];
            int count;
            UserType usertype;
            read_until(socket_, dynamicBuffer_, '\n');
            istream is(&dynamicBuffer_);
            getline(is, userData);
            splitString(userData, "-", userArrayData, count);
            if (checkUserIfExists(userArrayData[0], usertype))
            {
                if (usertype != DOKTER || usertype != PASIEN)
                {
                    write(socket_, buffer("USER_UPDATED \n"));
                    updateUserByUsername(userArrayData[0], userArrayData[1], convertStringToUserType(userArrayData[2]));
                    break;
                }
                else
                {
                    write(socket_, buffer("USER_CANT_CHANGE \n"));
                    continue;
                }
            }
            else
            {
                write(socket_, buffer("USER_FAIL \n"));
                continue;
            }
        }
    }
    void traversalUser() {}
    void allRiwayatTransaksi() {}
    void updatePasien() {}
    void deletePasien() {}
    void traversalPasien() {}
    void addDokter()
    {
        while (true)
        {
            string dokterData, dokterArrayData[4];
            int count;
            read_until(socket_, dynamicBuffer_, '\n');
            istream is(&dynamicBuffer_);
            getline(is, dokterData);
            splitString(dokterData, "-", dokterArrayData, count);
            if (isDokterExistByName(dokterArrayData[0]))
            {
                write(socket_, buffer("DOKTER_FAIL \n"));
                continue;
            }
            else
            {
                Dokter newDokter;
                newDokter.nama = dokterArrayData[0];
                newDokter.poliklinik = convertStringToPoliklinik(dokterArrayData[1]);
                newDokter.username = dokterArrayData[2];
                newDokter.password = dokterArrayData[3];
                saveUser(dokterArrayData[2], dokterArrayData[3], DOKTER);
                saveDokter(newDokter);
                write(socket_, buffer("DOKTER_SUCCESS \n"));
                break;
            }
        }
    }
    void updateDokter()
    {
        while (true)
        {
            string dokterData, dokterArrayData[4];
            int count;
            read_until(socket_, dynamicBuffer_, '\n');
            istream is(&dynamicBuffer_);
            getline(is, dokterData);
            splitString(dokterData, "-", dokterArrayData, count);
            if (isDokterExistByName(dokterArrayData[0]) && convertStringToPoliklinik(dokterArrayData[1]) != AFK)
            {
                Dokter newDokter;
                newDokter.nama = dokterArrayData[0];
                newDokter.poliklinik = convertStringToPoliklinik(dokterArrayData[1]);
                newDokter.username = dokterArrayData[2];
                newDokter.password = dokterArrayData[3];
                updateDokterByName(dokterArrayData[0], newDokter);
                write(socket_, buffer("DOKTER_UPDATED \n"));
                break;
            }
            else
            {
                write(socket_, buffer("DOKTER_FAIL \n"));
                continue;
            }
        }
    }
    void deleteDokter()
    {
        while (true)
        {
            string dokterName, username;
            int count;
            read_until(socket_, dynamicBuffer_, '\n');
            istream is(&dynamicBuffer_);
            getline(is, dokterName);
            if (isDokterExistByName(dokterName))
            {
                Dokter pdokter;
                readDokterByName(dokterName, pdokter);
                deleteUserByUsername(pdokter.username);
                deleteDokterByName(dokterName);

                write(socket_, buffer("DOKTER_FOUND \n"));
                break;
            }
            else
            {
                write(socket_, buffer("DOKTER_NOT_FOUND \n"));
                continue;
            }
        }
    }
    void traversalDokter() {}

    ip::tcp::socket socket_;
    boost::asio::streambuf dynamicBuffer_;
};

class TCPServer
{
public:
    TCPServer(io_service &ioService, short port)
        : acceptor_(ioService, ip::tcp::endpoint(ip::tcp::v4(), port)),
          socket_(ioService)
    {
        startAccept();
    }
    void shutdownServer()
    {
        acceptor_.close(); // Menutup acceptor
        socket_.close();   // Menutup socket
    }
    void inisiasiVariable()
    {
    }

private:
    void startAccept()
    {
        acceptor_.async_accept(socket_,
                               [this](const boost::system::error_code &ec)
                               {
                                   if (!ec)
                                   {
                                       cout << "Accepted connection from: "
                                            << socket_.remote_endpoint() << endl;

                                       // Handle the client in a separate thread
                                       thread(&TCPServer::handleClient, this,
                                              move(socket_))
                                           .detach();
                                   }

                                   // Continue to accept more connections
                                   startAccept();
                               });
    }

    void handleClient(ip::tcp::socket socket)
    {
        try
        {
            boost::asio::streambuf dynamicBuffer;
            string username, password;
            boost::asio::read_until(socket, dynamicBuffer, '\n');
            istream is(&dynamicBuffer);
            getline(is, username);

            boost::asio::read_until(socket, dynamicBuffer, '\n');
            is.clear();
            getline(is, password);

            // Verify user credentials using functions from passwordmanager.h
            UserType usertype;
            cout << password << " " << username << endl;
            if (authenticateUser(username, password, usertype))
            {
                string stringUserType = convertUserTypeToString(usertype);
                cout << "Login successful as " + stringUserType << endl;

                // Send login status to the client
                write(socket, buffer("Login Successful-" + stringUserType + "\n"));

                // Handle patient or kasir based on usertype
                if (usertype == PASIEN)
                {
                    handlePatient(move(socket), true);
                }
                else if (usertype == KASIR)
                {
                    handleKasir(move(socket));
                }
                else if (usertype == SUSTER)
                {
                    handleSuster(move(socket));
                }
                else if (usertype == DOKTER)
                {
                    handleDokter(move(socket));
                }
                else if (usertype == KASIR)
                {
                    handleKasir(move(socket));
                }
                else if (usertype == ADMIN)
                {
                    HandleAdmin adminHandler(move(socket));
                    adminHandler.startHandling();
                }
            }
            else
            {
                cout << "Login failed." << endl;

                // Send login status to the client
                write(socket, buffer("Login Failed\n"));
            }
        }
        catch (exception &e)
        {
            cerr << "Exception in thread: " << e.what() << endl;
        }
    }

    void handleKasir(ip::tcp::socket socket) {}
    void handlePatient(ip::tcp::socket socket, bool isOnline)
    {
        try
        {
            // Logic for handling patient queue
            boost::asio::streambuf dynamicBuffer;
            while (true)
            {
                cout << "terkoneksi" << endl;
                string request, requestArray[3];
                int count;
                boost::asio::read_until(socket, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, request);
                cout << "dapat pesan " << request << endl;
                splitString(request, "-", requestArray, count);

                if (requestArray[0].find("REGISTER") != string::npos)
                {
                    // User wants to register to the queue
                    PasienAntrianPendaftaran newPatient;
                    newPatient.nama = requestArray[1];
                    newPatient.pilihPoliklinik = requestArray[2];
                    newPatient.isOnline = isOnline;
                    newPatient.isDequeue = false;
                    registrationQueue_.enqueue(newPatient);

                    cout << "Patient registered: " << requestArray[1] << endl;

                    // Send response to the client
                    write(socket, buffer("Registration Successful, Silakan Tunggu\n"));
                    if (isOnline)
                    {
                        handleQueuePasien(move(socket), requestArray[1]);
                    }
                    else
                    {
                        handleSuster(move(socket));
                    }
                }
                else if (requestArray[0].find("CHECK_POLIKLINIK") != string::npos)
                {
                    // User wants to check poliklinik
                    if (checkDokterOnline(convertStringToPoliklinik(requestArray[1])))
                    {
                        write(socket, buffer("POLIKLINIK_READY\n"));
                    }
                    else
                    {
                        write(socket, buffer("POLIKLINIK_CLOSED\n"));
                    }
                }
            }
        }
        catch (exception &e)
        {
            cerr << "Exception in thread: " << e.what() << endl;
        }
    }

    void handleQueuePasien(ip::tcp::socket socket, string nama)
    {
        try
        {
            boost::asio::streambuf dynamicBuffer;
            while (true)
            {
                PasienAntrianPendaftaran patient = registrationQueue_.frontValue();
                if (patient.nama == nama && patient.isDequeue)
                {
                    // Mengirim pesan ke pasien
                    write(socket, buffer("Anda dipanggil!\n"));
                    registrationQueue_.dequeue();
                    // Menerima pesan dari pasien
                    string request;
                    boost::asio::read_until(socket, dynamicBuffer, '\n');
                    istream is(&dynamicBuffer);
                    getline(is, request);
                    if (request.find("RESET") != string::npos)
                    {
                        // Mengirim pesan ke pasien
                        registrationQueue_.enqueue(patient);
                        continue;
                    }
                    pasienQueue pasienStatus;
                    pasienStatus.isDequeue = false;
                    pasienStatus.isOnline = patient.isOnline;
                    pasienStatus.pilihanPoliklinik = patient.pilihPoliklinik;
                    handlePendaftaranSuster(move(socket), pasienStatus);
                    break;
                }

                // Sleep atau cara lain untuk menghindari busy-waiting
                this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        catch (exception &e)
        {
            cerr << "Exception in thread: " << e.what() << endl;
        }
    }

    void handlePoliklinikPasien(ip::tcp::socket socket)
    {
    }

    void handleKasirPasien(ip::tcp::socket socket)
    {
    }

    void handleSuster(ip::tcp::socket socket)
    {
        try
        {
            boost::asio::streambuf dynamicBuffer;
            while (true)
            {
                cout << "Terkoneksi suster" << endl;
                string request;
                boost::asio::read_until(socket, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, request);
                cout << "dapat pesan " << request << " dari suster" << endl;
                if (request.find("DEQUEUE") != string::npos)
                {
                    if (!registrationQueue_.isEmpty())
                    {
                        PasienAntrianPendaftaran oldPatient = registrationQueue_.frontValue();
                        PasienAntrianPendaftaran nextPatient = registrationQueue_.frontValue();
                        if (nextPatient.isOnline)
                        {
                            nextPatient.isDequeue = true;
                            registrationQueue_.updateItem(oldPatient, nextPatient);
                        }
                        else
                        {
                            registrationQueue_.dequeue();
                        }
                        // input si pilihan poliklinik
                        cout << "Pemanggilan pasien " << nextPatient.nama << endl;
                        string isOnlineString = (nextPatient.isOnline) ? "true" : "false";
                        write(socket, buffer("Pemanggilan pasien -" + nextPatient.nama + "-" + isOnlineString + "\n"));
                        pasienQueue pasienStatus;
                        pasienStatus.isOnline = nextPatient.isOnline;
                        pasienStatus.isDequeue = false;
                        pasienStatus.pilihanPoliklinik = oldPatient.pilihPoliklinik;
                        handlePendaftaranSuster(move(socket), pasienStatus);
                    }
                    else
                    {
                        // Menambah pesan ke antrean pasien yang kosong
                        write(socket, buffer("Antrian kosong. Mohon ditunggu. - \n"));
                    }
                }
                else if (request.find("REGISTER_OFFLINE") != string::npos)
                {
                    handlePatient(move(socket), false);
                }
            }
        }
        catch (exception &e)
        {
            cerr << "Exception in thread: " << e.what() << endl;
        }
    }

    void handlePendaftaranSuster(ip::tcp::socket socket, pasienQueue pasienStatus)
    {
        try
        {
            Pasien nextPasien;
            boost::asio::streambuf dynamicBuffer;
            while (true)
            {
                string request;
                read_until(socket, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, request);
                if (request.find("OLD_PASIEN") != string::npos)
                {
                    read_until(socket, dynamicBuffer, '\n');
                    istream is(&dynamicBuffer);
                    getline(is, request);
                    pasienStatus.nomorPasien = stoi(request);
                    // cari pasien berdasarkan nomor
                    if (searchdataPasienByID(nextPasien, pasienStatus.nomorPasien))
                    {
                        dataPribadi dataPasien;
                        dataPasien = nextPasien->dataPribadi;
                        write(socket, buffer("PASIEN_FOUND-" + dataPasien.nama + "\n"));
                        cout << "Selamat datang kembali, " + dataPasien.nama << endl;
                    }
                    else
                    {
                        write(socket, buffer("PASIEN_NOT_FOUND\n"));
                        continue;
                    }
                }
                else if (request.find("NEW_PASIEN") != string::npos)
                {
                    nextPasien = new pasien;
                    read_until(socket, dynamicBuffer, '\n');
                    istream is(&dynamicBuffer);
                    getline(is, request);
                    pasienStatus.nomorPasien = stoi(request);
                    nextPasien->nomor = pasienStatus.nomorPasien;
                    // cari pasien berdasarkan nomor
                    if (searchdataPasienByID(nextPasien, pasienStatus.nomorPasien))
                    {
                        write(socket, buffer("PASIEN_FOUND\n"));
                        continue;
                    }
                    else
                    {
                        int count;
                        string dataPasienArray[6];
                        dataPribadi pBaru;
                        read_until(socket, dynamicBuffer, '\n');
                        istream is(&dynamicBuffer);
                        getline(is, request);
                        splitString(request, "-", dataPasienArray, count);
                        pBaru.nama = dataPasienArray[0];
                        pBaru.alamat = dataPasienArray[1];
                        pBaru.kontak = dataPasienArray[2];
                        pBaru.tempatLahir = dataPasienArray[3];
                        pBaru.tanggalLahir = dataPasienArray[4];
                        pBaru.jenisKelamin = dataPasienArray[5];
                        nextPasien->dataPribadi = pBaru;
                        // masukkan kembali ke dataPasien
                        insertLastPasien(nextPasien);
                        write(socket, buffer("Pasien baru berhasil ditambahkan, Silakan lanjut ke Pemeriksaan Awal\n"));
                    }
                }
                else
                {
                    continue;
                }
                if (pasienStatus.isOnline)
                {
                    handlePoliklinikPasien(move(socket));
                }
                else
                {
                    handlePemeriksaanAwalSuster(move(socket), pasienStatus);
                }
            }
        }
        catch (exception &e)
        {
            cerr << "Exception in thread: " << e.what() << endl;
        }
    }

    void handlePemeriksaanAwalSuster(ip::tcp::socket socket, pasienQueue pasienStatus)
    {
        try
        {
            boost::asio::streambuf dynamicBuffer;
            while (true)
            {
                string dataPemeriksaanAwal, dataPemeriksaanAwalArray[5];
                int count, nomorPasien;
                pemeriksaanAwal pAwal;
                read_until(socket, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, dataPemeriksaanAwal);
                splitString(dataPemeriksaanAwal, "-", dataPemeriksaanAwalArray, count);
                pAwal.beratBadan = stoi(dataPemeriksaanAwalArray[0]);
                pAwal.tinggiBadan = stoi(dataPemeriksaanAwalArray[1]);
                pAwal.suhuBadan = stoi(dataPemeriksaanAwalArray[2]);
                pAwal.tekananDarah.sistolik = stoi(dataPemeriksaanAwalArray[3]);
                pAwal.tekananDarah.diastolik = stoi(dataPemeriksaanAwalArray[4]);
                insertPemeriksaanAwalByID(pAwal, pasienStatus.nomorPasien);
                Poliklinik pilihPoli = convertStringToPoliklinik(pasienStatus.pilihanPoliklinik);
                switch (pilihPoli)
                {
                case UMUM:
                    poliklinikUmumQueue_.enqueue(pasienStatus);
                    break;
                case MATA:
                    poliklinikMataQueue_.enqueue(pasienStatus);
                    break;
                case GIGI:
                    poliklinikGigiQueue_.enqueue(pasienStatus);
                    break;
                case THT:
                    poliklinikTHTQueue_.enqueue(pasienStatus);
                    break;
                default:
                    cout << "Error : Tidak ada poliklinik yang dipilih" << endl;
                    break;
                }
            }
        }
        catch (exception &e)
        {
            cerr << "Exception in thread: " << e.what() << endl;
        }
    }

    void handleDokter(ip::tcp::socket socket)
    {
        try
        {
            boost::asio::streambuf dynamicBuffer;
            while (true)
            {
                string request;
                read_until(socket, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, request);
                if (request.find("NEXT_PASIEN") != string::npos)
                {
                    //
                    // PR : buat kasih informasi pasien
                    //
                    write(socket, buffer("ceritanya data pasien\n"));
                    read_until(socket, dynamicBuffer, "\n");
                    string request, requestArray[10];
                    int count;
                    getline(is, request);
                    splitString(request, "-", requestArray, count);
                    if (requestArray[0].find("ADD_DIAGNOSA") != string::npos)
                    {
                        int nomorPasien;
                        Diagnosa pBantu = new diagnosa;
                        pBantu->hasilDiagnosa = requestArray[1];
                        pBantu->waktuDiagnosa = requestArray[2];
                        pBantu->next = NULL;
                        insertLastDiagnosa(pBantu, nomorPasien);
                    }
                }
            }
        }
        catch (exception &e)
        {
            cerr << "Exception in thread: " << e.what() << endl;
        }
    }

    void handlePoliklinikDokter(ip::tcp::socket socket, bool isOnline)
    {
    }

    void handleKasir(ip::tcp::socket socket, pasienQueue pasienStatus)
    {
        boost::asio::streambuf dynamicBuffer;
        read_until(socket, dynamicBuffer, "\n");
        string response;
        istream is(&dynamicBuffer);
        getline(is, response);
        // udh nerima pesan next pasien
    }
    // PR - fungsi yang dibutuhkan
    void insertPemeriksaanAwalByID(pemeriksaanAwal pBaru, int nomorPasien){};
    void insertLastDiagnosa(Diagnosa pBaru, int nomorPasien){};
    void insertLastPasien(Pasien pBaru){};
    bool checkDokterOnline(Poliklinik pilihPoliklinik) {}
    bool searchdataPasienByID(Pasien pBaru, int nomorPasien){};
    bool isDaftar, isPasienKasir;
    ip::tcp::acceptor acceptor_;
    ip::tcp::socket socket_;
    boost::asio::io_service ioService_;
    Queue<PasienAntrianPendaftaran> registrationQueue_;
    Queue<pasienQueue> poliklinikUmumQueue_, poliklinikMataQueue_, poliklinikTHTQueue_, poliklinikGigiQueue_, kasirQueue_;
    poliklinikStatus poliklinikArray[5] = {
        {UMUM, false},
        {MATA, false},
        {GIGI, false},
        {THT, false}};
};

int main()
{
    try
    {
        io_service ioService;
        TCPServer server(ioService, 8080);
        ioService.run();
    }
    catch (exception &e)
    {
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}
