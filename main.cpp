#include <iostream>
#include <stdexcept>
#include <string>
#include <limits>
#include <dlfcn.h>
#include "utils.hpp"

using namespace std;

typedef void (*cryptoAlg)(const string, const string);

bool processLibrary(const string libPath, const string encFunc, const string decFunc,
                    CryptoMode action, const string filePath, const string cryptoFilePath,
                    const string userPasswordHash) {
    void *handler = nullptr;
    try {
        handler = dlopen(libPath.c_str(), RTLD_LAZY);

        if (!handler) {
            throw runtime_error("Не удалось загрузить библиотеку: " + libPath);
        }

        if (action == CryptoMode::Encryption) {
            addUserHash(cryptoFilePath, userPasswordHash);
            cryptoAlg encrypt = (cryptoAlg) dlsym(handler, encFunc.c_str());
            if (!encrypt) {
                string error_detail = dlerror() ? dlerror() : "Unknown error";
                throw runtime_error("Функция шифрования (" + encFunc + ") не найдена в библиотеке. Ошибка: " + error_detail);
            }

            (*encrypt)(filePath, cryptoFilePath);
            cout << "Файл зашифрован!" << endl;
            
        } else {
            if (checkPasswordMatch(filePath, userPasswordHash)) {
                cryptoAlg decrypt = (cryptoAlg) dlsym(handler, decFunc.c_str());
                if (!decrypt) {
                    string error_detail = dlerror() ? dlerror() : "Unknown error";
                    throw runtime_error("Функция дешифрования (" + decFunc + ") не найдена в библиотеке. Ошибка: " + error_detail);
                }

                (*decrypt)(filePath, cryptoFilePath);
                cout << "Файл расшифрован!" << endl;
                
            } else {
                throw runtime_error("Неверный пароль или файл не был зашифрован!");
            }
        }

    } catch (const exception& e) {
        if (handler) {
            dlclose(handler);
        }
        throw; 
    }

    if (handler) {
        dlclose(handler);
    }
    return true;
}

int main() {
    clearScreen();
    while (true) {
        try {
            showMenu(MenuMode::StartMenu);
            
            Algorithm userAlgorithm = getCryptoAlgorithm();
            if (userAlgorithm == Algorithm::ExitProgram) {
                clearScreen();
                return 0;
            }
            
            string libPath;
            if (userAlgorithm == Algorithm::MatrixCipher) {
                libPath = "../lib/libMatrixCipher.so";
            } else if (userAlgorithm == Algorithm::VigenereCipher) {
                libPath = "../lib/libVigenereCipher.so";
            } else {
                libPath = "../lib/libPlayfairCipher.so";
            }
            
            void* handler = dlopen(libPath.c_str(), RTLD_LAZY);
            if (handler == NULL) {
                throw runtime_error("Не удалось загрузить библиотеку " + libPath);
            }
            dlclose(handler);

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
                    processLibrary("../lib/libMatrixCipher.so", "matrixEncrypt", "matrixDecrypt",
                                    action, filePath, cryptoFilePath, userPasswordHash);
                    break;

                case Algorithm::VigenereCipher:
                    processLibrary("../lib/libVigenereCipher.so", "vigenereEncrypt", "vigenereDecrypt",
                                    action, filePath, cryptoFilePath, userPasswordHash);
                    break;

                case Algorithm::PlayfairCipher:
                    processLibrary("../lib/libPlayfairCipher.so", "playfairEncrypt", "playfairDecrypt",
                                    action, filePath, cryptoFilePath, userPasswordHash);
                    break;
            }
            cout << "\n--------------------------------------\n\n";
        } catch (exception& e) {
            cerr << "[ ERROR ] " << e.what() << endl << endl;
        }
    }
}
