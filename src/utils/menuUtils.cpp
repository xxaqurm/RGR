#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <fstream>
#include <map>

#include "menuUtils.hpp"

using namespace std;
typedef unsigned long long ull;

OS getOs() {
    #if defined(__linux__)
        return OS::Linux;
    #elif defined(_WIN32)
        return OS::Windows;
    #else
        return OS::Unknown;
    #endif
}

void clearScreen() {
    /* Очищает терминал */
    if (getOs() == OS::Windows) {
        system("cls");
    } else if (getOs() == OS::Linux) {
        system("clear");
    } else {
        wcerr << L"Unknown system." << endl;
    }
}

void showMenu(const MenuMode mode) {
    /* Выводит меню пользователя на экран */
    switch (mode) {
        case MenuMode::StartMenu:
            wcout << L"Выберите алгоритм шифрования / дешифрования:" << endl;
            wcout << L"--- ВАЖНО! Алгоритм дешифрования должен совпадать с алгоритмом шифрования! ---" << endl;
            wcout << L"1. Матричная шифровка" << endl;
            wcout << L"2. Шифр Виженера" << endl;
            wcout << L"3. Шифр Плейфера" << endl;
            break;
        case MenuMode::EncDecMenu:
            wcout << L"Выберите действие:" << endl;
            wcout << L"1. Шифрование" << endl;
            wcout << L"2. Дешифрование" << endl;
            break;
        default:
            wcout << L"Something wrong." << endl;
            break;
    }
}

void userInputError(const wstring errorText, const exception e, bool ignore = true) {
    /* Выводит ошибку для пользователя */
    wcout << L">>> Ошибка: " << e.what() << L". " << errorText << endl;
    wcin.clear();
    if (ignore) wcin.ignore(numeric_limits<streamsize>::max(), '\n');
}

Algorithm getCryptoAlgorithm() {
    /* Пользовательский ввод (выбор алгоритма шифрования) */
    wstring inputError = L"Введите целое число от 0 до 3.";
    while (true) {
        wcout << L"Введите номер шифра или \"0\" для выхода: ";
        int userChoice = 0;
        try {
            wcin >> userChoice;
            if (userChoice < 0 || userChoice > 3) {
                throw invalid_argument("ivalid_user_choice");
            }
            wcin.clear();
            wcin.ignore(numeric_limits<streamsize>::max(), '\n');
            wcout << endl;

            Algorithm userAlgorithm = static_cast<Algorithm>(userChoice);
            return userAlgorithm;
        } catch (const ios::failure& e) {
            userInputError(inputError, e);
        } catch (const exception& e) {
            userInputError(inputError, e);
        }
    }
}

string getFilePath() {
    /* Получение пути файла */
    wstring inputError = L"Вы ввели неверный путь.";
    wstring wFilePath;
    while (true) {
        wcout << L"Введите полный путь до файла, путь не должен содержать кириллицу." << endl;
        
        OS os = getOs();
        if (os == OS::Linux) {
            wcout << L"(/home/usr/.../fileName): ";
        } else if (os == OS::Windows) {
            wcout << L"(C:\\Program Files\\...\\file): ";
        } else if (os == OS::Unknown) {
            wcout << L"Unknown OS!" << endl;
        } else {
            wcout << L"Something wrong (getFilePath function)" << endl;
        }

        try {
            getline(wcin, wFilePath);
            string filePath(wFilePath.begin(), wFilePath.end());
            wifstream inFile(filePath);
            if (!inFile) {
                throw invalid_argument("invalid_path");
            }
            return filePath;
        } catch (const exception& e) {
            userInputError(inputError, e, false);
        }
    }
}

CryptoMode getCryptoMod() {
    /* Пользовательский ввод (выбор действия (шифрование / дешифрование)) */
    wstring typeError = L"Введите целое число, а не строку.";
    wstring inputError = L"Введите 1 (шифрование) или 2 (дешифрование).";
    while (true) {
        wcout << L"Ваш выбор: ";
        int userChoice = 0;
        try {
            wcin >> userChoice;
            if (userChoice < 1 || userChoice > 2) {
                throw invalid_argument("invalid_crypto_mod");
            }
            wcin.clear();
            wcin.ignore(numeric_limits<streamsize>::max(), '\n');
            wcout << endl;

            userChoice--;
            CryptoMode cryptoMode = static_cast<CryptoMode>(userChoice);
            return cryptoMode;
        } catch (const ios::failure& e) {
            userInputError(typeError, e);
        } catch (const exception& e) {
            userInputError(inputError, e);
        }
    }
}

string removePostscript(const string filePath) {
    /* Удаляет приписку encrypted / decrypted*/
    int underlinePos = -1;
    int dotPos = -1;
    for (size_t i = 0; i < filePath.length(); i++) {
        if (filePath[i] == '_') {
            underlinePos = i;
        } else if (filePath[i] == '.') {
            dotPos = i;
        }
    }
    dotPos = dotPos == -1 ? filePath.length() : dotPos;

    string correctFilePath;
    for (size_t i = 0; i < filePath.length(); i++) {
        if (underlinePos <= i && i < dotPos) {
            continue;
        }
        correctFilePath += filePath[i];
    }

    return correctFilePath;
}

string createModFile(string filePath, const string postscript, const CryptoMode action) {
    /* Создает новый файл с припиской encrypted / decrypted */
    if (action == CryptoMode::Decryption) {
        filePath = removePostscript(filePath);
    }
    
    int dotPos = -1;
    for (size_t i = 0; i < filePath.length(); i++) {
        if (filePath[i] == '.') {
            dotPos = i;
        }
    }
    dotPos = dotPos == -1 ? filePath.length() : dotPos;

    string cryptoFilePath;
    for (size_t i = 0; i < filePath.length(); i++) {
        cryptoFilePath += filePath[i];
        if (i + 1 == dotPos) {
            cryptoFilePath += postscript;
        }
    }
    
    ofstream file(cryptoFilePath);
    return cryptoFilePath;
}

wstring getUserPassword(CryptoMode action) {
    /* Получает пароль пользователя */
    wstring passwordMismatchError = L"Пароли не совпадают. Попробуйте снова.";
    while (true) {
        try {
            wstring userPassword;
            while (userPassword.empty()) {
                wcout << L"Введите пароль для файла: ";
                getline(wcin, userPassword);
            }
 
            if (action == CryptoMode::Encryption) {
                wstring confirmPassword;
                while (confirmPassword.empty()) {
                    wcout << L"Подтвердите пароль: ";
                    getline(wcin, confirmPassword);
                }
                if (userPassword != confirmPassword) {
                    throw invalid_argument("mismatch");
                }
            }
            return userPassword;
        } catch (const exception& e) {
            userInputError(passwordMismatchError, e, false);
        }
    }
}

bool checkPasswordMatch(string filePath, wstring hash) {
    /* Проверяет совпадают ли хеши */
    OS os = getOs();
    string hashFilePath;
    if (os == OS::Linux) {
        int slashPos = -1;
        for (int i = 0; i < filePath.length(); i++) {
            if (filePath[i] == '/') {
                slashPos = i;
            }
        }
        
        for (int i = 0; i < filePath.length(); i++) {
            if (i <= slashPos) {
                hashFilePath += filePath[i];
            }
        }
    } else if (os == OS::Windows) {
        int backslashPos = -1;
        for (int i = 0; i < filePath.length(); i++) {
            if (filePath[i] == '\\') {
                backslashPos = i;
            }
        }

        for (int i = 0; i < filePath.length(); i++) {
            if (i <= backslashPos) {
                hashFilePath += filePath[i];
            }
        }
    }
    hashFilePath += "user_hash.txt";

    wifstream inFile(hashFilePath);

    wstring line;
    while (getline(inFile, line)) {
        wstring nowFilePath;
        wstring nowHash;

        bool spaceFound = false;
        
        for (auto& symb : line) {
            if (symb == L' ') {
                if (spaceFound) {
                    break;
                }
                spaceFound = true;
            } else if (spaceFound) {
                nowHash += symb;
            } else {
                nowFilePath += symb;
            }
        }
        string narrowFilePath(nowFilePath.begin(), nowFilePath.end());
        if (narrowFilePath == filePath && nowHash == hash) {
            return true;
        }
    }
    return false;
}

void addUserHash(string filePath, wstring hash) {
    /* Добавляем пароль пользователя */
    OS os = getOs();
    string hashFilePath;
    if (os == OS::Linux) {
        int slashPos = -1;
        for (int i = 0; i < filePath.length(); i++) {
            if (filePath[i] == '/') {
                slashPos = i;
            }
        }

        for (int i = 0; i < filePath.length(); i++) {
            if (i <= slashPos) {
                hashFilePath += filePath[i];
            }
        }
    } else if (os == OS::Windows) {
        int backslashPos = -1;
        for (int i = 0; i < filePath.length(); i++) {
            if (filePath[i] == '\\') {
                backslashPos = i;
            }
        }

        for (int i = 0; i < filePath.length(); i++) {
            if (i <= backslashPos) {
                hashFilePath += filePath[i];
            }
        }
    }
    hashFilePath += "user_hash.txt";

    wifstream inFile(hashFilePath);
    map<wstring, wstring> userData;
    
    bool updated = false;

    wstring line;
    while (getline(inFile, line)) {
        wstring nowFilePath;
        wstring nowHash;

        bool spaceFound = false;
        for (int i = 0; i < line.length(); i++) {
            if (line[i] == L' ') {
                spaceFound = true;
            } else if (spaceFound) {
                nowHash += line[i];
            } else {
                nowFilePath += line[i];
            }
        }
        string narrowFilePath(nowFilePath.begin(), nowFilePath.end());
        if (narrowFilePath == filePath) {
            nowHash = hash;
            updated = true;
        }
        userData[nowFilePath] = nowHash;
    }
    wstring wFilePath(filePath.begin(), filePath.end());
    if (!updated) {
        userData[wFilePath] = hash;
    } 

    wofstream outFile(hashFilePath);
    for (auto& [key, val] : userData) {
        outFile << key << " " << val << "\n";
    }
}

void simpleHash(wstring& userPassword) {
    ull hash = 7392;

    for (auto& symb : userPassword) {
        hash = ((hash << 5) + hash) + (ull)symb;
    }
    userPassword.clear();

    wstring char16 = L"0123456789abcdef";
    while (hash > 0) {
        int digit = hash % 16;
        userPassword = char16[digit] + userPassword;
        hash /= 16;
    }
}
