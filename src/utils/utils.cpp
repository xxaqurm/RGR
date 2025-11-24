#include "utils.hpp"

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
    while (true) {
        cout << "Введите номер шифра или \"0\" для выхода: ";
        int userChoice = 0;
        
        if (!(cin >> userChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "[ ERROR ] Выбор должен быть целым числом в диапазоне [0-3]\n" << endl;
            continue;
        }

        char nextChar = cin.peek();
        if (nextChar != '\n' && nextChar != EOF) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.clear();
            cerr << "[ ERROR ] Выбор должен быть целым числом в диапазоне [0-3]\n" << endl;
            continue;
        }
        
        if (userChoice < 0 || userChoice > 3) {
            cerr << "[ ERROR ] Выбор пользователя выходит за пределы диапазона [0-3]\n" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.clear();
        cout << endl;

        return static_cast<Algorithm>(userChoice);
    }
}

string getFilePath() {
    /* Получение пути файла */
    string inputError = "Вы ввели неверный путь.";
    string wFilePath;
    while (true) {
        cout << "Введите путь до файла (/home/usr/.../fileName): ";
        getline(cin, wFilePath);
        string filePath(wFilePath.begin(), wFilePath.end());
        ifstream inFile(filePath);
        if (!inFile) {
            cerr << "[ ERROR ] Введен неверный путь\n" << endl;
            continue;
        }
        return filePath;
    }
}

CryptoMode getCryptoMod() {
    /* Пользовательский ввод (выбор действия (шифрование / дешифрование)) */
    while (true) {
        cout << "Выберите действие: ";
        int userChoice = 0;

        if (!(cin >> userChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "[ ERROR ] Выбор должен быть целым числом в диапазоне [1-2]\n" << endl;
            continue;
        }

        char nextChar = cin.peek();
        if (nextChar != '\n' && nextChar != EOF) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.clear();
            cerr << "[ ERROR ] Выбор должен быть целым числом в диапазоне [1-2]\n" << endl;
            continue;
        }

        if (userChoice < 1 || userChoice > 2) {
            cerr << "[ ERROR ] Выбор выходит за пределы диапазона [1-2]\n" << endl;
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            cin.clear();
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.clear();
        cout << endl;

        return static_cast<CryptoMode>(userChoice);
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
    while (true) {
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
                cerr << "[ ERROR ] Пароли не совпадают. Попробуйте еще раз\n" << endl;
                userPassword.clear();
                confirmPassword.clear();
                cin.clear();
                continue;
            }
        }
        
        return userPassword;
    }
}

bool checkPasswordMatch(const string& filePath, const string& hash) {
    /* Проверяет совпадают ли хеши */
    string hashFilePath;
    bool slashExist = false;
    for (int i = filePath.length() - 1; i >= 0; i--) {
        if (filePath[i] == '/' && !slashExist) {
            slashExist = true;
        }

        if (slashExist) {
            hashFilePath = filePath[i] + hashFilePath;
        }
    }
    
    hashFilePath += "user_hash.txt";

    ifstream inFile(hashFilePath);

    string line;
    while (getline(inFile, line)) {
        string currentFilePath;
        string currentHash;

        bool spaceFound = false;
        for (int i = line.length() - 1; i >= 0; i--) {
            if (line[i] == ' ' && !spaceFound) {
                spaceFound = true;
                continue;
            }

            if (spaceFound) {
                currentFilePath = line[i] + currentFilePath;
            }
        }

        for (int i = line.length() - 1; i >= 0; i--) {
            if (line[i] == ' ') {
                break;
            }
            
            currentHash = line[i] + currentHash;
        }

        if (currentFilePath == filePath && currentHash == hash) {
            return true;
        } else if (currentFilePath == filePath && currentHash != hash) {
            return false;
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

string simpleHash(const string& userPassword) {
    uint64_t key = 7392;

    for (const char symb : userPassword) {
        key = ((key << 5) + key) + static_cast<uint64_t>(symb);
    }
    
    string hash;
    string char16 = "0123456789abcdef";
    while (key > 0) {
        int num = key % 16;
        hash = char16[num] + hash;
        key /= 16;
    }

    return hash;
}
