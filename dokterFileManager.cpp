#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "dokterManagement.cpp"

using namespace std;

bool isFileExist(string &filename)
{
    ifstream file(filename);
    return file.good();
}

void saveDokter(Dokter &dokter)
{
    ofstream file("dokter.dat", ios::app);
    if (!file.is_open())
    {
        cerr << "Error opening file for writing." << endl;
        return;
    }

    file << dokter.nama << "," << convertPoliklinikToString(dokter.poliklinik) << ","
         << dokter.username << "," << dokter.password << endl;

    cout << "Dokter saved successfully." << endl;

    file.close();
}

bool isDokterExistByName(string &nama)
{
    ifstream file("dokter.dat");
    if (!file.is_open())
    {
        cerr << "Error opening file for reading." << endl;
        return false;
    }

    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string storedNama;
        getline(ss, storedNama, ',');

        if (storedNama == nama)
        {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

bool readDokterByName(string &nama, Dokter &dokter)
{
    ifstream file("dokter.dat");
    if (!file.is_open())
    {
        cerr << "Error opening file for reading." << endl;
        return false;
    }

    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string storedNama, poliklinikStr, username, password;
        getline(ss, storedNama, ',');
        getline(ss, poliklinikStr, ',');
        getline(ss, username, ',');
        getline(ss, password, ',');

        if (storedNama == nama)
        {
            dokter.nama = storedNama;
            dokter.poliklinik = convertStringToPoliklinik(poliklinikStr);
            dokter.username = username;
            dokter.password = password;

            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

bool updateDokterByName(string &nama, Dokter &updatedDokter)
{
    ifstream inFile("dokter.dat");
    ofstream outFile("temp.dat", ios::out);

    if (!inFile.is_open() || !outFile.is_open())
    {
        cerr << "Error opening file for reading or writing." << endl;
        return false;
    }

    string line;
    bool dokterFound = false;

    while (getline(inFile, line))
    {
        stringstream ss(line);
        string storedNama, poliklinikStr, username, password;
        getline(ss, storedNama, ',');
        getline(ss, poliklinikStr, ',');
        getline(ss, username, ',');
        getline(ss, password, ',');

        if (storedNama == nama)
        {
            outFile << updatedDokter.nama << "," << convertPoliklinikToString(updatedDokter.poliklinik) << ","
                    << updatedDokter.username << "," << updatedDokter.password << endl;
            dokterFound = true;
        }
        else
        {
            outFile << line << endl;
        }
    }

    inFile.close();
    outFile.close();

    remove("dokter.dat");
    rename("temp.dat", "dokter.dat");

    return dokterFound;
}

bool deleteDokterByName(string &nama)
{
    ifstream inFile("dokter.dat");
    ofstream outFile("temp.dat", ios::out);

    if (!inFile.is_open() || !outFile.is_open())
    {
        cerr << "Error opening file for reading or writing." << endl;
        return false;
    }

    string line;
    bool dokterFound = false;

    while (getline(inFile, line))
    {
        stringstream ss(line);
        string storedNama, poliklinikStr, username, password;
        getline(ss, storedNama, ',');
        getline(ss, poliklinikStr, ',');
        getline(ss, username, ',');
        getline(ss, password, ',');

        if (storedNama == nama)
        {
            dokterFound = true;
        }
        else
        {
            outFile << line << endl;
        }
    }

    inFile.close();
    outFile.close();

    remove("dokter.dat");
    rename("temp.dat", "dokter.dat");

    return dokterFound;
}
