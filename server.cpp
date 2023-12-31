// server.cpp
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <thread>
#include <conio.h>
#include <ctime>
#include <csignal>
// #include "passwordmanager.cpp"
// #include "dokterFileManager.cpp"
#include "database.cpp"
#include "queue.cpp"
// #include "struct.cpp"
#include "etc.cpp"

using namespace std;
using namespace boost::asio;

void handleSignal(int signal)
{
    cout << "[Server] Mematikan server ..." << endl;
    exit(signal);
}

DatabaseManager *DatabaseManager::instance = NULL;

class HandleAdmin
{
public:
    HandleAdmin(ip::tcp::socket socket) : socket_(std::move(socket))
    {
    }

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
                else if (Command.find("ADD_NEW_DOKTER") != string::npos)
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
            if (dbManager.checkUserIfExists(userArrayData[0], usertype))
            {
                write(socket_, buffer("USER_FAIL \n"));
                continue;
            }
            else
            {
                write(socket_, buffer("USER_SUCCESS \n"));
                UserData newUserData;
                newUserData.username = userArrayData[0];
                newUserData.password = userArrayData[1];
                newUserData.usertype = convertStringToUserType(userArrayData[2]);
                dbManager.addUserdata(newUserData);
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
            if (dbManager.checkUserIfExists(username, usertype))
            {
                dbManager.deleteUserdataByUsername(username);
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
            if (dbManager.checkUserIfExists(userArrayData[0], usertype))
            {
                if (usertype != DOKTER || usertype != PASIEN)
                {
                    write(socket_, buffer("USER_UPDATED \n"));
                    UserData newUserData;
                    newUserData.username = userArrayData[0];
                    newUserData.password = userArrayData[1];
                    newUserData.usertype = convertStringToUserType(userArrayData[2]);
                    dbManager.updateUserdataByUsername(userArrayData[0], newUserData);
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
            if (dbManager.isDokterExistByName(dokterArrayData[0]))
            {
                write(socket_, buffer("DOKTER_FAIL \n"));
                continue;
            }
            Dokter newDokter;
            newDokter.nama = dokterArrayData[0];
            newDokter.poliklinik = convertStringToPoliklinik(dokterArrayData[1]);
            newDokter.username = dokterArrayData[2];
            newDokter.password = dokterArrayData[3];
            UserData newUserData;
            newUserData.username = dokterArrayData[2];
            newUserData.password = dokterArrayData[3];
            newUserData.usertype = DOKTER;
            dbManager.addUserdata(newUserData);
            dbManager.addDokter(newDokter);
            write(socket_, buffer("DOKTER_SUCCESS \n"));
            break;
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
            if (dbManager.isDokterExistByName(dokterArrayData[0]) && convertStringToPoliklinik(dokterArrayData[1]) != AFK)
            {
                Dokter newDokter;
                newDokter.nama = dokterArrayData[0];
                newDokter.poliklinik = convertStringToPoliklinik(dokterArrayData[1]);
                newDokter.username = dokterArrayData[2];
                newDokter.password = dokterArrayData[3];
                dbManager.updateDokterByUsername(dokterArrayData[0], newDokter);
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
            if (dbManager.isDokterExistByName(dokterName))
            {
                Dokter pdokter;
                dbManager.readDokterByUsername(dokterName, pdokter);
                dbManager.deleteUserdataByUsername(pdokter.username);
                dbManager.deleteDokterByUsername(dokterName);

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

    DatabaseManager &dbManager = DatabaseManager::getInstance();
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

            UserType usertype;
            if (dbManager.verifyUserdata(username, password, usertype))
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
                else if (usertype == SUSTER)
                {
                    handleSuster(move(socket));
                }
                else if (usertype == DOKTER)
                {
                    Dokter dataDokter;
                    dbManager.readDokterByUsername(username, dataDokter);
                    handleDokter(move(socket), dataDokter);
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

    void handlePatient(ip::tcp::socket socket, bool isOnline)
    {
        try
        {
            // Logic for handling patient queue
            boost::asio::streambuf dynamicBuffer;
            while (true)
            {
                string request, requestArray[3];
                int count;
                read_until(socket, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, request);
                splitString(request, "-", requestArray, count);

                if (requestArray[0].find("REGISTER") != string::npos)
                {
                    PasienAntrianPendaftaran newPatient;
                    newPatient.nama = requestArray[1];
                    newPatient.pilihPoliklinik = requestArray[2];
                    newPatient.isOnline = isOnline;
                    newPatient.isDequeue = false;
                    registrationQueue_.enqueue(newPatient);

                    cout << "[Pasien] Pasien " << newPatient.nama << " ditambahkan ke dalam pendaftaran queue" << endl;

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
                    cout << requestArray[0];
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
                    cout << "[Pasien] Pasien " << patient.nama << " dipanggil ke pendaftaran" << endl;
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

    void handlePoliklinikPasien(ip::tcp::socket socket, pasienQueue pasienStatus)
    {
        try
        {
            boost::asio::streambuf dynamicBuffer;
            Poliklinik pilihan = convertStringToPoliklinik(pasienStatus.pilihanPoliklinik);
            pasienQueue patient;
            while (true)
            {

                switch (pilihan)
                {
                case UMUM:
                    patient = poliklinikUmumQueue_.frontValue();
                    break;
                case GIGI:
                    patient = poliklinikGigiQueue_.frontValue();
                    break;
                case MATA:
                    patient = poliklinikMataQueue_.frontValue();
                    break;
                case THT:
                    patient = poliklinikTHTQueue_.frontValue();
                    break;
                default:
                    break;
                }
                if (patient.nomorPasien == pasienStatus.nomorPasien && patient.isDequeue)
                {
                    write(socket, buffer("CALL_PASIEN_POLIKLINIK\n"));
                    cout << "[Pasien] Pasien dengan nomor " << pasienStatus.nomorPasien << " dipanggil ke poliklinik (" << pasienStatus.pilihanPoliklinik << ")" << endl;
                    handleStopPoliklinikPasien(move(socket), pasienStatus);
                }
                else
                    // Sleep atau cara lain untuk menghindari busy-waiting
                    this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        catch (exception &e)
        {
            cerr << "Exception in thread: " << e.what() << endl;
        }
    }

    void handleStopPoliklinikPasien(ip::tcp::socket socket, pasienQueue pasienStatus)
    {
        try
        {
            boost::asio::streambuf dynamicBuffer;
            Poliklinik pilihan = convertStringToPoliklinik(pasienStatus.pilihanPoliklinik);
            pasienQueue patient;
            while (true)
            {
                switch (pilihan)
                {
                case UMUM:
                    patient = poliklinikUmumQueue_.frontValue();
                    break;
                case GIGI:
                    patient = poliklinikGigiQueue_.frontValue();
                    break;
                case MATA:
                    patient = poliklinikMataQueue_.frontValue();
                    break;
                case THT:
                    patient = poliklinikTHTQueue_.frontValue();
                    break;
                default:
                    break;
                }
                if (patient.nomorPasien == pasienStatus.nomorPasien && patient.isDequeue2)
                {
                    // Mengirim pesan ke pasien
                    write(socket, buffer("PASIEN_POLIKLINIK_DONE\n"));
                    pasienStatus.isDequeue2 = false;
                    switch (pilihan)
                    {
                    case UMUM:
                        poliklinikUmumQueue_.dequeue();
                        break;
                    case GIGI:
                        poliklinikGigiQueue_.dequeue();
                        break;
                    case MATA:
                        poliklinikMataQueue_.dequeue();
                        break;
                    case THT:
                        poliklinikTHTQueue_.dequeue();
                        break;
                    default:
                        break;
                    }
                    handleKasirPasien(move(socket), pasienStatus);
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

    void handleKasirPasien(ip::tcp::socket socket, pasienQueue pasienStatus)
    {
        try
        {
            boost::asio::streambuf dynamicBuffer;
            pasienQueue patient;
            while (true)
            {
                patient = kasirQueue_.frontValue();
                if (patient.nomorPasien == pasienStatus.nomorPasien && patient.isDequeue)
                {
                    // Mengirim pesan ke pasien
                    write(socket, buffer("CALL_PASIEN_KASIR\n"));
                    cout << "[Pasien] Pasien dengan nomor " << pasienStatus.nomorPasien << " dipanggil ke kasir" << endl;
                    handleStopKasirPasien(move(socket), pasienStatus);
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
    void handleStopKasirPasien(ip::tcp::socket socket, pasienQueue pasienStatus)
    {
        try
        {
            boost::asio::streambuf dynamicBuffer;
            pasienQueue patient;
            while (true)
            {
                patient = kasirQueue_.frontValue();
                if (patient.nomorPasien == pasienStatus.nomorPasien && patient.isDequeue2)
                {
                    // Mengirim pesan ke pasien
                    write(socket, buffer("PASIEN_KASIR_DONE\n"));
                    socket.close();
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

    void handleSuster(ip::tcp::socket socket)
    {
        try
        {
            boost::asio::streambuf dynamicBuffer;
            while (true)
            {
                string request;
                boost::asio::read_until(socket, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, request);
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
                        cout << "[SUSTER] Pemanggilan pasien " << nextPatient.nama << endl;
                        string isOnlineString = (nextPatient.isOnline) ? "true" : "false";
                        write(socket, buffer("Pemanggilan pasien -" + nextPatient.nama + "-" + isOnlineString + "\n"));
                        pasienQueue pasienStatus;
                        pasienStatus.namaAwal = nextPatient.nama;
                        pasienStatus.isOnline = nextPatient.isOnline;
                        pasienStatus.isDequeue = false;
                        pasienStatus.isDequeue2 = false;
                        pasienStatus.pilihanPoliklinik = oldPatient.pilihPoliklinik;
                        pemeriksaanAwalQueue_.enqueue(pasienStatus);
                        if (pasienStatus.isOnline)
                        {
                            handlePendaftaranOnlineSuster(move(socket), pasienStatus);
                        }
                        else
                        {
                            handlePendaftaranSuster(move(socket), pasienStatus);
                        }
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

    void handlePendaftaranOnlineSuster(ip::tcp::socket socket, pasienQueue pasienStatus)
    {
        try
        {
            boost::asio::streambuf dynamicBuffer;
            while (true)
            {

                pasienQueue oldPasien, nextPasien;
                oldPasien = nextPasien = pemeriksaanAwalQueue_.frontValue();
                if (oldPasien.namaAwal == pasienStatus.namaAwal && oldPasien.isDequeue)
                {
                    // Mengirim pesan ke pasien
                    write(socket, buffer("REGIS_DONE\n"));
                    nextPasien.isDequeue = false;
                    nextPasien.isDequeue2 = false;
                    pemeriksaanAwalQueue_.updateItem(oldPasien, nextPasien);
                    handlePemeriksaanAwalSuster(move(socket), pasienStatus);
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

    void handlePemeriksaanAwalOnline(ip::tcp::socket socket, pasienQueue pasienStatus)
    {
        try
        {
            boost::asio::streambuf dynamicBuffer;
            while (true)
            {
                pasienQueue patient = pemeriksaanAwalQueue_.frontValue();
                if (patient.nomorPasien == pasienStatus.nomorPasien && patient.isDequeue2)
                {
                    // Mengirim pesan ke pasien
                    write(socket, buffer("PEMERIKSAAN_AWAL_DONE-" + pasienStatus.pilihanPoliklinik + "\n"));
                    write(socket, buffer(createStringFromPemeriksaanAwal(patient.dataPasien.pemeriksaanAwal)));
                    pemeriksaanAwalQueue_.dequeue();
                    pasienStatus.isDequeue2 = false;
                    pasienStatus.isDequeue = false;
                    handlePoliklinikPasien(move(socket), pasienStatus);
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

    void handlePendaftaranSuster(ip::tcp::socket socket, pasienQueue pasienStatus)
    {
        try
        {
            Pasien nextPasien, dumpPasien;
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
                    pasienStatus.nomorPasien = extractNumber(request);
                    // cari pasien berdasarkan nomor
                    if (dbManager.ReadPasienByID(pasienStatus.nomorPasien, nextPasien))
                    {
                        write(socket, buffer("PASIEN_FOUND-" + nextPasien.dataPribadi.nama + "\n"));
                        cout << "[Suster] Selamat datang kembali, Pasien " + nextPasien.dataPribadi.nama << endl;
                    }
                    else
                    {
                        write(socket, buffer("PASIEN_NOT_FOUND\n"));
                        continue;
                    }
                }
                else if (request.find("NEW_PASIEN") != string::npos)
                {

                    int count;
                    string dataPasienArray[6];
                    read_until(socket, dynamicBuffer, '\n');
                    istream is(&dynamicBuffer);
                    getline(is, request);
                    splitString(request, "-", dataPasienArray, count);
                    nextPasien.dataPribadi.nama = dataPasienArray[0];
                    nextPasien.dataPribadi.alamat = dataPasienArray[1];
                    nextPasien.dataPribadi.kontak = dataPasienArray[2];
                    nextPasien.dataPribadi.tempatLahir = dataPasienArray[3];
                    nextPasien.dataPribadi.tanggalLahir = dataPasienArray[4];
                    nextPasien.dataPribadi.jenisKelamin = dataPasienArray[5];
                    nextPasien.diagnosaPasien = NULL;
                    dbManager.insertPasien(nextPasien, pasienStatus.nomorPasien);
                    write(socket, buffer("Pasien baru berhasil ditambahkan\n"));
                    cout << "[Suster] Menambahkan pasien baru atas nama " + nextPasien.dataPribadi.nama << endl;
                }
                else
                {
                    continue;
                }
                if (pasienStatus.isOnline)
                {
                    pasienQueue nextPasienQueue, oldPasien;
                    nextPasienQueue = oldPasien = pemeriksaanAwalQueue_.frontValue();
                    nextPasienQueue.nomorPasien = pasienStatus.nomorPasien;
                    nextPasienQueue.dataPasien = nextPasien;
                    nextPasienQueue.isDequeue = true;
                    pemeriksaanAwalQueue_.updateItem(oldPasien, nextPasienQueue);
                    handlePemeriksaanAwalOnline(move(socket), pasienStatus);
                    break;
                }
                else
                {
                    handlePemeriksaanAwalSuster(move(socket), pasienStatus);
                    break;
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
                pAwal.beratBadan = extractNumber(dataPemeriksaanAwalArray[0]);
                pAwal.tinggiBadan = extractNumber(dataPemeriksaanAwalArray[1]);
                pAwal.suhuBadan = extractNumber(dataPemeriksaanAwalArray[2]);
                pAwal.tekananDarah.sistolik = extractNumber(dataPemeriksaanAwalArray[3]);
                pAwal.tekananDarah.diastolik = extractNumber(dataPemeriksaanAwalArray[4]);

                pasienQueue nextPasien, oldPasien;
                nextPasien = oldPasien = pemeriksaanAwalQueue_.frontValue();
                nextPasien.dataPasien.pemeriksaanAwal = pAwal;
                nextPasien.isDequeue = false;
                nextPasien.isDequeue2 = true;
                pemeriksaanAwalQueue_.updateItem(oldPasien, nextPasien);

                Poliklinik pilihPoli = convertStringToPoliklinik(pasienStatus.pilihanPoliklinik);
                switch (pilihPoli)
                {
                case UMUM:
                    poliklinikUmumQueue_.enqueue(nextPasien);
                    break;
                case MATA:
                    poliklinikMataQueue_.enqueue(nextPasien);
                    break;
                case GIGI:
                    poliklinikGigiQueue_.enqueue(nextPasien);
                    break;
                case THT:
                    poliklinikTHTQueue_.enqueue(nextPasien);
                    break;
                default:
                    cout << "Error : Tidak ada poliklinik yang dipilih" << endl;
                    break;
                }
                handleSuster(move(socket));
            }
        }
        catch (exception &e)
        {
            cerr << "Exception in thread: " << e.what() << endl;
        }
    }

    void handleDokter(ip::tcp::socket socket, Dokter dataDokter)
    {
        try
        {
            boost::asio::streambuf dynamicBuffer;
            changePoliklikStatus(dataDokter.poliklinik, true);
            write(socket, buffer(dataDokter.nama + "#" + convertPoliklinikToString(dataDokter.poliklinik) + "\n"));
            Pasien nextPasien;
            while (true)
            {
                string request;
                read_until(socket, dynamicBuffer, '\n');
                istream is(&dynamicBuffer);
                getline(is, request);
                if (request.find("NEXT_PASIEN") != string::npos)
                {
                    bool isNotEmpty;
                    pasienQueue pasienStatus;
                    switch (dataDokter.poliklinik)
                    {
                    case UMUM:
                        isNotEmpty = denqueueAntrianPasien(poliklinikUmumQueue_, pasienStatus);
                        break;
                    case GIGI:
                        isNotEmpty = denqueueAntrianPasien(poliklinikGigiQueue_, pasienStatus);
                        break;
                    case MATA:
                        isNotEmpty = denqueueAntrianPasien(poliklinikMataQueue_, pasienStatus);
                        break;
                    case THT:
                        isNotEmpty = denqueueAntrianPasien(poliklinikTHTQueue_, pasienStatus);
                        break;
                    default:
                        break;
                    }
                    if (!isNotEmpty)
                    {
                        write(socket, buffer("EMPTY_QUEUE\n"));
                        continue;
                    }
                    else
                    {
                        write(socket, buffer("QUEUE\n"));
                    }

                    string dataPribadiString = createStringFromDataPribadi(pasienStatus.dataPasien.dataPribadi);
                    string pemeriksaanAwalString = createStringFromPemeriksaanAwal(pasienStatus.dataPasien.pemeriksaanAwal);
                    write(socket, buffer(dataPribadiString + "\n"));
                    write(socket, buffer(pemeriksaanAwalString + "\n"));

                    read_until(socket, dynamicBuffer, "\n");
                    string request, requestArray[10];
                    int count;
                    getline(is, request);
                    cout << request << endl;
                    splitString(request, "#", requestArray, count);
                    if (requestArray[0].find("ADD_DIAGNOSA") != string::npos)
                    {
                        pasienStatus.dataPasien.diagnosaPasien = NULL;
                        Diagnosa pBantu = new diagnosa;
                        pBantu->hasilDiagnosa = requestArray[1];
                        pBantu->waktuDiagnosa = getTimeToString();
                        Treatment pBantuTreatmemt;
                        pBantu->treatmentDiagnosa = NULL;
                        bool isError;
                        do
                        {
                            isError = false;
                            count = 0;
                            read_until(socket, dynamicBuffer, "\n");
                            is.clear();
                            getline(is, request);
                            cout << request << endl;
                            splitString(request, "#", requestArray, count);
                            cout << requestArray[0] << endl;
                            if (requestArray[0].find("ADD_TREATMENT") != string::npos)
                            {
                                pBantuTreatmemt = new treatment;
                                pBantuTreatmemt->namaTreatment = requestArray[1];
                                pBantuTreatmemt->biaya = extractLongNumber(requestArray[2]);
                                insertLastTreatment(pBantu->treatmentDiagnosa, pBantuTreatmemt);
                            }
                            else if (requestArray[0].find("STOP_TREATMENT") != string::npos)
                            {
                                isError = true;
                            }
                        } while (!isError);
                        insertLastDiagnosa(pasienStatus.dataPasien.diagnosaPasien, pBantu);
                        cout << pasienStatus.dataPasien.diagnosaPasien->id << endl;
                        if (pasienStatus.isOnline)
                        {
                            pasienQueue oldPasien;
                            switch (convertStringToPoliklinik(pasienStatus.pilihanPoliklinik))
                            {
                            case UMUM:
                                oldPasien = poliklinikUmumQueue_.frontValue();
                                pasienStatus.isDequeue = false;
                                pasienStatus.isDequeue2 = true;
                                poliklinikUmumQueue_.updateItem(oldPasien, pasienStatus);
                                break;
                            case MATA:
                                oldPasien = poliklinikMataQueue_.frontValue();
                                pasienStatus.isDequeue = false;
                                pasienStatus.isDequeue2 = true;
                                poliklinikMataQueue_.updateItem(oldPasien, pasienStatus);
                                break;
                            case GIGI:
                                oldPasien = poliklinikGigiQueue_.frontValue();
                                pasienStatus.isDequeue = false;
                                pasienStatus.isDequeue2 = true;
                                poliklinikGigiQueue_.updateItem(oldPasien, pasienStatus);
                                break;
                            case THT:
                                oldPasien = poliklinikTHTQueue_.frontValue();
                                pasienStatus.isDequeue = false;
                                pasienStatus.isDequeue2 = true;
                                poliklinikTHTQueue_.updateItem(oldPasien, pasienStatus);
                                break;

                            default:
                                break;
                            }
                        }
                        dbManager.updatePasienByID(pasienStatus.nomorPasien, pasienStatus.dataPasien);
                        pasienStatus.isDequeue = false;
                        pasienStatus.isDequeue2 = false;
                        pasienStatus.dokter = dataDokter.nama;
                        kasirQueue_.enqueue(pasienStatus);
                    }
                }
            }
        }
        catch (exception &e)
        {
            changePoliklikStatus(dataDokter.poliklinik, false);
            cerr << "Exception in thread: " << e.what() << endl;
        }
    }

    void handleKasir(ip::tcp::socket socket)
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
                cout << request;
                if (request.find("NEXT_PASIEN") != string::npos)
                {
                    pasienQueue pasienStatus;
                    bool isNotEmpty = denqueueAntrianPasien(kasirQueue_, pasienStatus);
                    if (!isNotEmpty)
                    {
                        write(socket, buffer("EMPTY_QUEUE\n"));
                    }
                    else
                    {
                        write(socket, buffer("QUEUE\n"));
                        string dataPribadiString = createStringFromDataPribadi(pasienStatus.dataPasien.dataPribadi);
                        string diagnosaString = createStringFromDiagnosa(pasienStatus.dataPasien.diagnosaPasien);
                        cout << dataPribadiString << "\n"
                             << /*diagnosaString <<*/ endl;
                        write(socket, buffer(dataPribadiString + "\n"));
                        write(socket, buffer(diagnosaString + "\n"));
                        write(socket, buffer(to_string(pasienStatus.nomorPasien) + "#" + pasienStatus.pilihanPoliklinik + "#" + pasienStatus.dokter + "\n"));
                        read_until(socket, dynamicBuffer, "\n");
                        string request, requestArray[4];
                        int count;
                        getline(is, request);
                        cout << request;
                        splitString(request, "#", requestArray, count);
                        if (requestArray[0].find("ADD_TRANSAKSI") != string::npos)
                        {
                            riwayatTransaksi rt;
                            rt.idPasien = pasienStatus.nomorPasien;
                            rt.namaPasien = pasienStatus.dataPasien.dataPribadi.nama;
                            rt.total = extractLongNumber(requestArray[1]);
                            rt.jumlahDibayarkan = extractLongNumber(requestArray[2]);
                            rt.waktuTransaksi = getTimeToString();
                            rt.treatmentPasien = pasienStatus.dataPasien.diagnosaPasien->treatmentDiagnosa;
                            dbManager.insertRiwayatTransaksi(rt);
                            if (pasienStatus.isOnline)
                            {
                                pasienQueue oldPasien = kasirQueue_.frontValue();
                                pasienStatus.isDequeue = false;
                                pasienStatus.isDequeue2 = true;
                                kasirQueue_.updateItem(oldPasien, pasienStatus);
                            }
                        }
                    }
                }
            }
        }
        catch (exception &e)
        {
            cerr << "Exception in thread: " << e.what() << endl;
        }
    }

    bool denqueueAntrianPasien(Queue<pasienQueue> &pasienStatusQueue, pasienQueue &pasienStatus)
    {
        pasienQueue oldPasien;
        if (!pasienStatusQueue.isEmpty())
        {
            oldPasien = pasienStatus = pasienStatusQueue.frontValue();
            if (oldPasien.isOnline)
            {
                pasienStatus.isDequeue = true;
                pasienStatus.isDequeue2 = false;
                pasienStatusQueue.updateItem(oldPasien, pasienStatus);
            }
            else
            {
                pasienStatusQueue.dequeue();
            }
            return true;
        }
        else
        {
            // Menambah pesan ke antrean pasien yang kosong
            return false;
        }
    }
    void changePoliklikStatus(Poliklinik dokterPoliklinik, bool statusPoliklinik)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (poliklinikArray_[i].poliklinik == dokterPoliklinik)
            {
                string status = (statusPoliklinik) ? "Buka" : "Tutup";
                poliklinikArray_[i].isReady = statusPoliklinik;
                cout << "[Poliklinik] status Poliklinik " + convertPoliklinikToString(poliklinikArray_[i].poliklinik) + ": " + status << endl;
            }
        }
    };
    bool checkDokterOnline(Poliklinik pilihPoliklinik)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (poliklinikArray_[i].poliklinik == pilihPoliklinik)
            {
                return poliklinikArray_[i].isReady;
            }
        }
        return false;
    }
    string getTimeToString()
    {
        time_t now = time(0);
        tm *localTime = localtime(&now);
        char bufferTime[80];
        strftime(bufferTime, sizeof(bufferTime), "%Y-%m-%d %H:%M:%S", localTime);
        string waktu_sekarang(bufferTime);
        return waktu_sekarang;
    }

    // Global variable
    DatabaseManager &dbManager = DatabaseManager::getInstance();
    ip::tcp::acceptor acceptor_;
    ip::tcp::socket socket_;
    boost::asio::io_service ioService_;
    Queue<PasienAntrianPendaftaran> registrationQueue_;
    Queue<pasienQueue> pemeriksaanAwalQueue_, poliklinikUmumQueue_, poliklinikMataQueue_, poliklinikTHTQueue_, poliklinikGigiQueue_, kasirQueue_;
    poliklinikStatus poliklinikArray_[5] = {
        {UMUM, false},
        {MATA, false},
        {GIGI, false},
        {THT, false}};
};

int main()
{
    system("cls");
    signal(SIGINT, handleSignal);
    welcomeScreen();
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
