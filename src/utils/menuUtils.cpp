#include "menuUtils.hpp"

void clearScreen() {
    system("clear");
}

void showMenu(const MenuMode mode) {
    /* Выводит меню пользователя на экран */
    switch (mode) {
        case MenuMode::StartMenu:
            cout << "Выберите алгоритм шифрования / дешифрования:" << endl;
            cout << "--- ВАЖНО! Алгоритм дешифрования должен совпадать с алгоритмом шифрования! ---" << endl;
            cout << "1. Матричная шифровка" << endl;
            cout << "2. Шифр Виженера" << endl;
            cout << "3. Шифр Плейфера" << endl;
            break;
        case MenuMode::EncDecMenu:
            cout << "Выберите действие:" << endl;
            cout << "1. Шифрование" << endl;
            cout << "2. Дешифрование" << endl;
            break;
        default:
            cout << "[ ERROR ] что-то пошло не так" << endl;
            break;
    }
}

void userInputError(const string errorText, const exception e, bool ignore = true) {
    /* Выводит ошибку для пользователя */
    cout << "[ ERROR ] " << e.what() << ". " << errorText << endl;
    cin.clear();
    if (ignore) cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

Algorithm getCryptoAlgorithm() {
    /* Пользовательский ввод (выбор алгоритма шифрования) */
    string inputError = "Введите целое число от 0 до 3.";
    while (true) {
        cout << "Введите номер шифра или \"0\" для выхода: ";
        int userChoice = 0;
        try {
            cin >> userChoice;
            if (userChoice < 0 || userChoice > 3) {
                throw invalid_argument("ivalid_user_choice");
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << endl;

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
    string inputError = "Вы ввели неверный путь.";
    string wFilePath;
    while (true) {
        cout << "Введите путь до файла (/home/usr/.../fileName): ";

        try {
            getline(cin, wFilePath);
            string filePath(wFilePath.begin(), wFilePath.end());
            ifstream inFile(filePath);
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
    string typeError = "Введите целое число, а не строку.";
    string inputError = "Введите 1 (шифрование) или 2 (дешифрование).";
    while (true) {
        cout << "Ваш выбор: ";
        int userChoice = 0;
        try {
            cin >> userChoice;
            if (userChoice < 1 || userChoice > 2) {
                throw invalid_argument("invalid_crypto_mod");
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << endl;

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

string getUserPassword(CryptoMode action) {
    /* Получает пароль пользователя */
    string passwordMismatchError = "Пароли не совпадают. Попробуйте снова.";
    while (true) {
        try {
            string userPassword;
            while (userPassword.empty()) {
                cout << "Введите пароль для файла: ";
                getline(cin, userPassword);
            }
 
            if (action == CryptoMode::Encryption) {
                string confirmPassword;
                while (confirmPassword.empty()) {
                    cout << "Подтвердите пароль: ";
                    getline(cin, confirmPassword);
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

bool checkPasswordMatch(string filePath, string hash) {
    /* Проверяет совпадают ли хеши */
    string hashFilePath;
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
    
    hashFilePath += "user_hash.txt";

    ifstream inFile(hashFilePath);

    string line;
    while (getline(inFile, line)) {
        string nowFilePath;
        string nowHash;

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

void addUserHash(string filePath, string hash) {
    /* Добавляем пароль пользователя */
    string hashFilePath;
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
    hashFilePath += "user_hash.txt";

    ifstream inFile(hashFilePath);
    map<string, string> userData;
    
    bool updated = false;

    string line;
    while (getline(inFile, line)) {
        string nowFilePath;
        string nowHash;

        bool spaceFound = false;
        for (int i = 0; i < line.length(); i++) {
            if (line[i] == ' ') {
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
    string wFilePath(filePath.begin(), filePath.end());
    if (!updated) {
        userData[wFilePath] = hash;
    } 

    ofstream outFile(hashFilePath);
    for (auto& [key, val] : userData) {
        outFile << key << " " << val << "\n";
    }
}

void simpleHash(string& userPassword) {
    uint64_t hash = 7392;

    for (auto& symb : userPassword) {
        hash = ((hash << 5) + hash) + (uint64_t)symb;
    }
    userPassword.clear();

    string char16 = "0123456789abcdef";
    while (hash > 0) {
        int digit = hash % 16;
        userPassword = char16[digit] + userPassword;
        hash /= 16;
    }
}
