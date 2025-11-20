#include <iostream>
#include <stdexcept>
#include <string>
#include <limits>
#include <dlfcn.h>
#include "menuUtils.hpp"

using namespace std;

bool processLibrary(const string libPath, const string encFunc, const string decFunc,
                   CryptoMode action, const string filePath, const string cryptoFilePath,
                   const string userPasswordHash) {
    void *handler = dlopen(libPath.c_str(), RTLD_LAZY);
    if (!handler) {
        cerr << "[ Debug ] Library " << libPath.c_str() << " not found." << endl;
        return false;
    }

    typedef void cryptoAlg(const string, const string);

    if (action == CryptoMode::Encryption) {
        addUserHash(cryptoFilePath, userPasswordHash);

        cryptoAlg *encrypt = (cryptoAlg*) dlsym(handler, encFunc.c_str());
        if (!encrypt) {
            cerr << "[ Debug ] Function " << encFunc.c_str() << " not found." << endl;
            dlclose(handler);
            return false;
        }

        (*encrypt)(filePath, cryptoFilePath);
        cout << "Файл зашифрован!" << endl;
    }
    else {
        if (checkPasswordMatch(filePath, userPasswordHash)) {
            cryptoAlg *decrypt = (cryptoAlg*) dlsym(handler, decFunc.c_str());
            if (!decrypt) {
                cerr << "[ Debug ] Function " << decFunc.c_str() << " not found." << endl;
                dlclose(handler);
                return false;
            }

            (*decrypt)(filePath, cryptoFilePath);
            cout << "Файл расшифрован!" << endl;
        } 
        else {
            cout << "Неверный пароль или этот файл не был зашифрован!" << endl;
            dlclose(handler);
            return false;
        }
    }

    dlclose(handler);
    return true;
}

int main() {
    clearScreen();
    while (true) {
        showMenu(MenuMode::StartMenu);
        
        Algorithm userAlgorithm = getCryptoAlgorithm();
        if (userAlgorithm == Algorithm::ExitProgram) {
            clearScreen();
            return 0;
        }

        showMenu(MenuMode::EncDecMenu);
        CryptoMode action = getCryptoMod();
        
        string filePath = getFilePath();
        string cryptoPostscript;
        if (action == CryptoMode::Encryption) {
            cryptoPostscript = "_encrypted";
        } else {
            cryptoPostscript = "_decrypted";
        }

        string cryptoFilePath = createModFile(filePath, cryptoPostscript, action);
        string userPasswordHash = getUserPassword(action);        
        simpleHash(userPasswordHash);

        switch(userAlgorithm) {
            case Algorithm::MatrixCipher:
                processLibrary("./lib/libMatrixCipher.so", "matrixEncrypt", "matrixDecrypt",
                            action, filePath, cryptoFilePath, userPasswordHash);
                break;

            case Algorithm::VigenereCipher:
                processLibrary("./lib/libVigenereCipher.so", "vigenereEncrypt", "vigenereDecrypt",
                            action, filePath, cryptoFilePath, userPasswordHash);
                break;

            case Algorithm::PlayfairCipher:
                processLibrary("./lib/libPlayfairCipher.so", "playfairEncrypt", "playfairDecrypt",
                            action, filePath, cryptoFilePath, userPasswordHash);
                break;
        }
        cout << "\n--------------------------------------\n\n";
    }
}
