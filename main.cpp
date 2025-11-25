#include <iostream>
#include <stdexcept>
#include <string>
#include <limits>
#include <vector>
#include <dlfcn.h>
#include "utils.hpp"

using namespace std;

typedef vector<uint8_t> (*cryptoAlg)(const string&, const string&, const string&);

bool libraryExsist(const string& libPath) {
    void* handler = dlopen(libPath.c_str(), RTLD_LAZY);
    if (handler == NULL) {
        cerr << "[ ERROR ] Не удалось загрузить библиотеку " + libPath << endl;
        dlclose(handler);
        return false;
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

        showMenu(MenuMode::EncryptionIOMode);
        IOMode userIOMode = getIOMode();
        
        string libPath;
        string encFunc;
        if (userAlgorithm == Algorithm::MatrixCipher) {
            libPath = "../lib/libMatrixCipher.so";
            encFunc = "matrix";
        } else if (userAlgorithm == Algorithm::VigenereCipher) {
            libPath = "../lib/libVigenereCipher.so";
            encFunc = "vigenere";
        } else {
            libPath = "../lib/libPlayfairCipher.so";
            encFunc = "playfair";
        }

        if (!libraryExsist(libPath)) {
            continue;
        }

        showMenu(MenuMode::EncDecMenu);
        CryptoMode action = getCryptoMod();

        if (userIOMode == IOMode::Console) {
            if (!libraryExsist(libPath)) {
                cerr << "[ ERROR ] Не удалось загрузить библиотеку: " + libPath << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            void* handler = dlopen(libPath.c_str(), RTLD_LAZY);

            if (action == CryptoMode::Encryption) {
                encFunc += "Encrypt";
                cryptoAlg encrypt = (cryptoAlg) dlsym(handler, encFunc.c_str());
                if (!encrypt) {
                    dlclose(handler);
                    cerr << "[ ERROR ] Функция шифрования (" + encFunc + ") не найдена" << endl;
                    continue;
                }

                string userCryptoText;
                cout << "Введите свой текст: ";
                getline(cin, userCryptoText);
                vector<uint8_t> output = (*encrypt)("default", "default", userCryptoText);
                
                cout << "Текст зашифрован: ";
                for (const uint8_t b : output) {
                    cout << static_cast<int>(b) << " ";
                }
                
                cout << "\n\n--------------------------------------\n\n";
            } else {
                encFunc += "Decrypt";
                cryptoAlg decrypt = (cryptoAlg) dlsym(handler, encFunc.c_str());
                if (!decrypt) {
                    dlclose(handler);
                    cerr << "[ ERROR ] Функция дешифрования (" + encFunc + ") не найдена" << endl;
                    continue;
                }

                string userCryptoText;
                cout << "Введите свой текст: ";
                getline(cin, userCryptoText);
                vector<uint8_t> output = (*decrypt)("default", "default", userCryptoText);
                
                cout << "Текст расшифрован" << endl;
                for (const uint8_t b : output) {
                    cout << b;
                }
                
                cout << "\n\n--------------------------------------\n\n";
            }
        } else {
            string filePath = getFilePath();
            string cryptoPostscript;
            if (action == CryptoMode::Encryption) {
                cryptoPostscript = "_encrypted";
            } else {
                cryptoPostscript = "_decrypted";
            }

            string cryptoFilePath = createModFile(filePath, cryptoPostscript, action);
            
            string userPassword;
            string userPasswordHash;
            if (action == CryptoMode::Encryption) {
                userPassword = getUserPassword(action);
                userPasswordHash = simpleHash(userPassword);
                userPassword.clear();        
            } else {
                int maxAttempts = 5;
                int cntAttempts = 0;
                while (cntAttempts != maxAttempts) {
                    userPassword = getUserPassword(action);
                    userPasswordHash = simpleHash(userPassword);
                    
                    if (checkPasswordMatch(filePath, userPasswordHash)) {
                        break;
                    }
                    
                    cntAttempts++;
                    cout << "Пароль неверный. Количество попыток: " << cntAttempts << "/" << maxAttempts << endl;
                }

                if (cntAttempts == maxAttempts) {
                    cout << endl;
                    continue;
                }
            }

            if (!libraryExsist(libPath)) {
                cerr << "[ ERROR ] Не удалось загрузить библиотеку: " << libPath << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            void* handler = dlopen(libPath.c_str(), RTLD_LAZY);
            switch(action) {
                case CryptoMode::Encryption: {
                    addUserHash(cryptoFilePath, userPasswordHash);
                    
                    encFunc += "Encrypt";
                    cryptoAlg encrypt = (cryptoAlg) dlsym(handler, encFunc.c_str());
                    
                    if (!encrypt) {
                        dlclose(handler);
                        cerr << "[ ERROR ] Функция шифрования (" << encFunc << ") не найдена в библиотеке: " << libPath << endl;
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        break;
                    }

                    (*encrypt)(filePath, cryptoFilePath, "default");
                    cout << "Файл зашифрован" << endl;
                    break;
                }

                case CryptoMode::Decryption: {
                    encFunc += "Decrypt";
                    cryptoAlg decrypt = (cryptoAlg) dlsym(handler, encFunc.c_str());
                    if (!decrypt) {
                        dlclose(handler);
                        cerr << "[ ERROR ] Функция дешифрования (" << encFunc << ") не найдена в библиотеке: " << libPath << endl;
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        break;
                    }

                    (*decrypt)(filePath, cryptoFilePath, "default");
                    cout << "Файл дешифрован" << endl;
                    break;
                }

                default: {
                    cerr << "[ ERROR ] Неизвестный метод" << endl;
                    break;
                }
            }
            cout << "\n--------------------------------------\n\n";
        }
    }
}
