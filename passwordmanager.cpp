#include <iostream>
#include <fstream>
#include <string>
#include "userManagement.cpp"

bool fileExists(const std::string &filename)
{
    std::ifstream file(filename);
    return file.good();
}

bool checkUserIfExists(const std::string &username, UserType &usertype)
{
    const std::string filepath = "passwords.dat";
    if (!fileExists(filepath))
    {
        std::cerr << "passwords.dat file does not exist. User does not exist." << std::endl;
        return false;
    }

    std::ifstream file(filepath);

    if (!file.is_open())
    {
        std::cerr << "Error opening file for reading." << std::endl;
        return false;
    }

    std::string storedUsername, storedPassword, userTypeStr;

    while (file >> storedUsername >> storedPassword >> userTypeStr)
    {
        if (storedUsername == username)
        {
            file.close();
            usertype = convertStringToUserType(userTypeStr);
            return true; // User already exists
        }
    }

    file.close();
    return false; // User does not exist
}

void deleteUserByUsername(const std::string &username)
{
    const std::string filepath = "passwords.dat";
    if (!fileExists(filepath))
    {
        std::cerr << "passwords.dat file does not exist. Cannot delete user." << std::endl;
        return;
    }

    std::ifstream inputFile(filepath);
    std::ofstream tempFile("temp.dat");

    if (!inputFile.is_open() || !tempFile.is_open())
    {
        std::cerr << "Error opening files for reading/writing." << std::endl;
        return;
    }

    std::string storedUsername, storedPassword, userTypeStr;

    while (inputFile >> storedUsername >> storedPassword >> userTypeStr)
    {
        if (storedUsername != username)
        {
            tempFile << storedUsername << " " << storedPassword << " " << userTypeStr << std::endl;
        }
    }

    inputFile.close();
    tempFile.close();

    // Remove the original file
    if (std::remove(filepath.c_str()) != 0)
    {
        std::cerr << "Error deleting original file." << std::endl;
        return;
    }

    // Rename the temp file to the original file
    if (std::rename("temp.dat", filepath.c_str()) != 0)
    {
        std::cerr << "Error renaming temp file." << std::endl;
        return;
    }

    std::cout << "User with username " << username << " deleted successfully." << std::endl;
}

void updateUserByUsername(const std::string &username, const std::string &newPassword, UserType newUserType)
{
    const std::string filepath = "passwords.dat";
    if (!fileExists(filepath))
    {
        std::cerr << "passwords.dat file does not exist. Cannot update user." << std::endl;
        return;
    }

    std::ifstream inputFile(filepath);
    std::ofstream tempFile("temp.dat");

    if (!inputFile.is_open() || !tempFile.is_open())
    {
        std::cerr << "Error opening files for reading/writing." << std::endl;
        return;
    }

    std::string storedUsername, storedPassword, userTypeStr;

    while (inputFile >> storedUsername >> storedPassword >> userTypeStr)
    {
        if (storedUsername == username)
        {
            tempFile << storedUsername << " " << newPassword << " ";
            switch (newUserType)
            {
            case ADMIN:
                tempFile << "ADMIN";
                break;
            case DOKTER:
                tempFile << "DOKTER";
                break;
            case SUSTER:
                tempFile << "SUSTER";
                break;
            case KASIR:
                tempFile << "KASIR";
                break;
            case PASIEN:
                tempFile << "PASIEN";
                break;
            default:
                tempFile << "UNKNOWN";
                break;
            }
            tempFile << std::endl;
        }
        else
        {
            tempFile << storedUsername << " " << storedPassword << " " << userTypeStr << std::endl;
        }
    }

    inputFile.close();
    tempFile.close();

    // Remove the original file
    if (std::remove(filepath.c_str()) != 0)
    {
        std::cerr << "Error deleting original file." << std::endl;
        return;
    }

    // Rename the temp file to the original file
    if (std::rename("temp.dat", filepath.c_str()) != 0)
    {
        std::cerr << "Error renaming temp file." << std::endl;
        return;
    }

    std::cout << "User with username " << username << " updated successfully." << std::endl;
}

void saveUser(const std::string &username, const std::string &password, UserType usertype)
{
    if (!fileExists("passwords.dat"))
    {
        std::ofstream createFile("passwords.dat");
        if (!createFile.is_open())
        {
            std::cerr << "Error creating passwords.dat file." << std::endl;
            return;
        }
        createFile.close();
    }

    std::ofstream file("passwords.dat", std::ios::binary | std::ios::app);

    if (!file.is_open())
    {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }

    file << username << " " << password << " ";
    switch (usertype)
    {
    case ADMIN:
        file << "ADMIN";
        break;
    case DOKTER:
        file << "DOKTER";
        break;
    case SUSTER:
        file << "SUSTER";
        break;
    case KASIR:
        file << "KASIR";
        break;
    case PASIEN:
        file << "PASIEN";
        break;
    default:
        file << "UNKNOWN";
        break;
    }
    file << std::endl;

    std::cout << "Password saved successfully." << std::endl;

    file.close();
}

bool authenticateUser(const std::string &username, const std::string &password, UserType &usertype)
{
    const std::string filepath = "passwords.dat";
    if (!fileExists(filepath))
    {
        std::cerr << "passwords.dat file does not exist. Authentication failed." << std::endl;
        return false;
    }

    std::ifstream file(filepath);

    if (!file.is_open())
    {
        std::cerr << "Error opening file for reading." << std::endl;
        return false;
    }

    std::string storedUsername, storedPassword, userTypeStr;

    while (file >> storedUsername >> storedPassword >> userTypeStr)
    {
        if ((storedUsername.compare(username) == 0) && (storedPassword.compare(password) == 0))
        {
            file.close();
            if (userTypeStr == "ADMIN")
                usertype = ADMIN;
            else if (userTypeStr == "DOKTER")
                usertype = DOKTER;
            else if (userTypeStr == "SUSTER")
                usertype = SUSTER;
            else if (userTypeStr == "KASIR")
                usertype = KASIR;
            else if (userTypeStr == "PASIEN")
                usertype = PASIEN;
            else
                usertype = UserType(-1); // Nilai default jika tidak dikenali
            return true;                 // Username dan password sesuai
        }
    }

    file.close();
    return false; // Username atau password tidak sesuai
}
