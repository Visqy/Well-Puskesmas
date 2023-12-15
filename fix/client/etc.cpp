#include <iostream>
#include <chrono>
#include <future>
#include <thread>
#include <limits>
#include <sstream>
#include <string>

using namespace std;

bool waitForAnyKey(char &pressedKey, int timeoutSeconds)
{
    auto startTime = std::chrono::steady_clock::now();
    pressedKey = '\0';

    // Use a future to store the result of the asynchronous operation
    auto future = async(launch::async, [&pressedKey]()
                        {
        // Check if a key is pressed
        if (cin.peek() != EOF)
        {
            cin.get(pressedKey);
            return true;
        }
        return false; });

    while (true)
    {
        // Check if the timeout has been reached
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

        cout << "Elapsed time: " << elapsedTime << " seconds." << endl;

        if (elapsedTime >= timeoutSeconds)
        {
            cout << "Timeout reached." << endl;
            return false;
        }

        // Pause for a short duration before checking again
        this_thread::sleep_for(std::chrono::milliseconds(100));

        // Check if the asynchronous operation is completed
        if (future.wait_for(std::chrono::seconds(0)) == future_status::ready)
        {
            return future.get(); // Return the result of the asynchronous operation
        }
    }
}

void splitString(const string &input, const string &delimiter, string result[], int &count)
{
    // Inisialisasi variabel count
    count = 0;

    // Menggunakan stringstream untuk memisahkan string
    stringstream ss(input);
    string token;

    // Loop memisahkan string menggunakan delimiter
    while (getline(ss, token, delimiter[0]))
    {
        result[count++] = token;
    }
}

int extractNumber(string &inputString)
{
    int result = 0;

    for (char character : inputString)
    {
        if (isdigit(character))
        {
            result = result * 10 + (character - '0');
        }
    }

    return result;
}

long extractLongNumber(string &inputString)
{
    long result = 0;

    for (char character : inputString)
    {
        if (isdigit(character))
        {
            result = result * 10 + (character - '0');
        }
    }

    return result;
}