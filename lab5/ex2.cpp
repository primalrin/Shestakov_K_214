#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <limits.h>
#include <algorithm>
#include <stdexcept>

using namespace std;

enum ret_type_t
{
    SUCCESS,
    ERROR_OPEN_FILE,
    ERROR_SAME_FILES,
    ERROR_NO_KEY,
    ERROR_INVALID_FILE_PATH,
    ERROR_FILE_OPERATION,
    ERROR_MEMORY_ALLOCATION,
    ERROR_INVALID_ARGUMENT,
};

void logErrors(int code)
{
    switch (code)
    {
    case SUCCESS:
        break;
    case ERROR_OPEN_FILE:
        cerr << "Error: Failed to open file." << endl;
        break;
    case ERROR_SAME_FILES:
        cerr << "Error: Input and output file paths point to the same file. Please specify different paths." << endl;
        break;
    case ERROR_NO_KEY:
        cerr << "Error: Encryption key cannot be empty." << endl;
        break;
    case ERROR_INVALID_FILE_PATH:
        cerr << "Error: Invalid file path." << endl;
        break;
    case ERROR_FILE_OPERATION:
        cerr << "Error: File operation failed." << endl;
        break;
    case ERROR_MEMORY_ALLOCATION:
        cerr << "Error: Memory allocation failed." << endl;
        break;
    case ERROR_INVALID_ARGUMENT:
        cerr << "Error: Invalid argument passed to function." << endl;
        break;
    default:
        cerr << "Error: An unknown error occurred (code: " << code << ")." << endl;
        break;
    }
}

class Encoder
{
public:
    Encoder(const vector<byte> &_key) : key(_key)
    {
        if (key.empty())
        {
            throw invalid_argument("Encryption key cannot be empty.");
        }
    }

    Encoder(const Encoder &other) : key(other.key) {}
    Encoder &operator=(const Encoder &other)
    {
        if (this != &other)
        {
            key = other.key;
        }
        return *this;
    }
    ~Encoder() = default;

    int encode(const string &inputFilePath, const string &outputFilePath, bool encrypt) const
    {
        if (inputFilePath.empty() || outputFilePath.empty())
        {
            return ERROR_INVALID_FILE_PATH;
        }

        if (areFilesSame(inputFilePath, outputFilePath))
        {
            return ERROR_SAME_FILES;
        }

        ifstream inputFile(inputFilePath, ios::binary);
        if (!inputFile.is_open())
        {
            return ERROR_OPEN_FILE;
        }

        ofstream outputFile(outputFilePath, ios::binary);
        if (!outputFile.is_open())
        {
            inputFile.close();
            return ERROR_OPEN_FILE;
        }

        if (key.empty())
        {
            inputFile.close();
            outputFile.close();
            return ERROR_NO_KEY;
        }

        vector<byte> S(256);
        try
        {
            for (int i = 0; i < 256; ++i)
            {
                S[i] = static_cast<byte>(i);
            }
        }
        catch (const exception &e)
        {
            inputFile.close();
            outputFile.close();
            return ERROR_MEMORY_ALLOCATION;
        }

        int j = 0;
        for (int i = 0; i < 256; ++i)
        {
            j = (j + to_integer<int>(S[i]) + to_integer<int>(key[i % key.size()])) % 256;
            swap(S[i], S[j]);
        }

        int i = 0, k = 0;
        char byte_in;
        char byte_out;
        while (inputFile.get(byte_in))
        {
            i = (i + 1) % 256;
            j = (j + to_integer<int>(S[i])) % 256;
            swap(S[i], S[j]);
            k = to_integer<int>(S[(to_integer<int>(S[i]) + to_integer<int>(S[j])) % 256]);

            byte_out = byte_in ^ static_cast<char>(k);

            if (!outputFile.put(byte_out))
            {
                inputFile.close();
                outputFile.close();
                return ERROR_FILE_OPERATION;
            }
        }

        inputFile.close();
        outputFile.close();

        return SUCCESS;
    }

    int setKey(const vector<byte> &newKey)
    {
        if (newKey.empty())
        {
            return ERROR_INVALID_ARGUMENT;
        }
        key = newKey;
        return SUCCESS;
    }

private:
    vector<byte> key;

    bool areFilesSame(const string &filePath1, const string &filePath2) const
    {
        if (filePath1 == filePath2)
            return true;
        struct stat stat1, stat2;

        if (stat(filePath1.c_str(), &stat1) != 0)
        {
            return false;
        }

        if (stat(filePath2.c_str(), &stat2) != 0)
        {
            return false;
        }

        return stat1.st_ino == stat2.st_ino && stat1.st_dev == stat2.st_dev;
    }
};

int main()
{
    vector<byte> key = {byte(0x01), byte(0x23), byte(0x45), byte(0x67), byte(0x89), byte(0xAB), byte(0xCD), byte(0xEF)};

    Encoder *encoder = nullptr;
    try
    {
        encoder = new Encoder(key);
        if (!encoder)
        {
            logErrors(ERROR_MEMORY_ALLOCATION);
            return ERROR_MEMORY_ALLOCATION;
        }
    }
    catch (const invalid_argument &e)
    {
        cerr << "Initialization error: " << e.what() << endl;
        return ERROR_INVALID_ARGUMENT;
    }
    catch (const bad_alloc &e)
    {
        logErrors(ERROR_MEMORY_ALLOCATION);
        return ERROR_MEMORY_ALLOCATION;
    }
    catch (...)
    {
        cerr << "Unknown initialization error." << endl;
        return -1;
    }

    int code = encoder->encode("input.txt", "encrypted.txt", true);
    if (code != SUCCESS)
    {
        logErrors(code);
        delete encoder;
        return code;
    }

    code = encoder->encode("encrypted.txt", "decrypted.txt", false);
    if (code != SUCCESS)
    {
        logErrors(code);
        delete encoder;
        return code;
    }

    cout << "Encryption and decryption processes completed successfully." << endl;

    delete encoder;
    return SUCCESS;
}