#pragma once

#include <iostream>
#include <limits>
#include <stdexcept>
#include <fstream>
#include <map>
#include <cstdint>

using namespace std;

enum class Algorithm {
    ExitProgram,
    MatrixCipher,
    VigenereCipher,
    PlayfairCipher
};

enum class MenuMode {
    StartMenu,
    EncDecMenu
};

enum class CryptoMode {
    Encryption,
    Decryption
};

extern "C" {
    void clearScreen();
    void showMenu(const MenuMode mode);

    CryptoMode getCryptoMod();
    Algorithm getCryptoAlgorithm();

    string createModFile(string filePath, const string postscript, const CryptoMode crypMode);
    string getFilePath();
    string getUserPassword(CryptoMode action);

    void addUserHash(string filePath, string hash);
    void simpleHash(string& userPassword);

    bool checkPasswordMatch(string filePath, string hash);
}
