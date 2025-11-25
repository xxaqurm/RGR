#include "VigenereCipher.hpp"

ByteArray getKeyBytes() {
    string key;
    while (key.empty()) {
        cout << "Введите ключ: ";
        getline(cin, key);
    }
    return ByteArray(key.begin(), key.end());
}

ByteArray vigenereEncryptBytes(const ByteArray data, const ByteArray key) {
    ByteArray result(data.size());

    for (size_t i = 0; i < data.size(); i++) {
        result[i] = static_cast<uint8_t>((data[i] + key[i % key.size()]) % 256);
    }

    return result;
}

ByteArray vigenereDecryptBytes(const ByteArray& data, const ByteArray& key) {
    ByteArray result(data.size());

    for (size_t i = 0; i < data.size(); i++) {
        result[i] = static_cast<uint8_t>((data[i] - key[i % key.size()] + 256) % 256);
    }
    return result;
}

ByteArray vigenereEncrypt(const string& filePath, const string& fileEncryptedPath, const string& userCryptoText) {
    ByteArray fileContent;
    if (filePath == "default" && fileEncryptedPath == "default" && userCryptoText != "default") {
        for (const uint8_t b : userCryptoText) {
            fileContent.push_back(b);
        }
    } else {
        ifstream inFile(filePath, ios::binary);
        if (!inFile) {
            cerr << "[ ERROR ] Ошибка открытия файла: " << filePath << endl;
            return ByteArray{};
        }

        inFile.seekg(0, ios::end);
        size_t size_ = inFile.tellg();
        inFile.seekg(0, ios::beg);

        fileContent.resize(size_);
        inFile.read(reinterpret_cast<char*>(fileContent.data()), size_);
    }

    ByteArray key = getKeyBytes();
    ByteArray output = vigenereEncryptBytes(fileContent, key);

    if (filePath == "default" && fileEncryptedPath == "default" && userCryptoText != "default") {
        return output;
    } else {
        ofstream outFile(fileEncryptedPath, ios::binary);
        if (!outFile) {
            cerr << "[ ERROR ] Ошибка открытия файла: " << fileEncryptedPath << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return ByteArray{};
        }
        outFile.write(reinterpret_cast<const char*>(output.data()), output.size());
    }

    return ByteArray{};
}

ByteArray vigenereDecrypt(const string& filePath, const string& fileDecryptedPath, const string& userCryptoText) {
    ByteArray fileContent;
    if (filePath == "default" && fileDecryptedPath == "default" && userCryptoText != "default") {
        uint8_t b = 0;
        for (size_t i = 0; i < userCryptoText.length(); i++) {
            char c = userCryptoText[i];
            if (c == ' ' || i + 1 == userCryptoText.length()) {
                if (i + 1 == userCryptoText.length()) {
                    b = b * 10 + (c - '0');
                }
                fileContent.push_back(b);
                b = 0;
                continue;
            }
            b = b * 10 + (c - '0');
        }
    } else {
        ifstream inFile(filePath, ios::binary);
        if (!inFile) {
            cerr << "[ ERROR ] Ошибка открытия файла: " << string(filePath.begin(), filePath.end()) << endl;
            return ByteArray{};
        }

        inFile.seekg(0, ios::end);
        size_t size_ = inFile.tellg();
        inFile.seekg(0, ios::beg);

        fileContent.resize(size_);
        inFile.read(reinterpret_cast<char*>(fileContent.data()), size_);
    }

    ByteArray key = getKeyBytes();
    ByteArray output = vigenereDecryptBytes(fileContent, key);

    if (filePath == "default" && fileDecryptedPath == "default" && userCryptoText != "default") {
        return output;
    } else {
        ofstream outFile(fileDecryptedPath, ios::binary);
        if (!outFile) {
            cerr << "[ ERROR ] Ошибка открытия файла: " << fileDecryptedPath << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return ByteArray{};
        }
        outFile.write(reinterpret_cast<const char*>(output.data()), output.size());
    }

    return ByteArray{};
}
