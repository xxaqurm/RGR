#pragma once

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <fstream>

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

enum class OS {
    Windows,
    Linux,
    Unknown
};

extern "C" {
    void clearScreen();
    void showMenu(const MenuMode mode);

    CryptoMode getCryptoMod();
    Algorithm getCryptoAlgorithm();
    OS getOs();

    string createModFile(string filePath, const string postscript, const CryptoMode crypMode);
    string getFilePath();
    wstring getUserPassword(CryptoMode action);

    void addUserHash(string filePath, wstring hash);
    void simpleHash(wstring& userPassword);

    bool checkPasswordMatch(string filePath, wstring hash);
}
