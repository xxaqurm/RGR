#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#include "VigenereCipher.hpp"

using namespace std;

typedef uint8_t Byte;
typedef vector<Byte> ByteArray;

ByteArray getKeyBytes() {
    /* Преобразует строку-ключ в вектор байтов */
    wstring key;
    while (key.empty()) {
        wcout << L"Введите ключевое слово для шифрования/дешифрования файла: ";
        getline(wcin, key);
    }

    return ByteArray(key.begin(), key.end());
}

ByteArray vigenereEncryptBytes(const ByteArray data, const ByteArray key) {
    /* Шифр Виженера для байтов */
    ByteArray result(data.size());

    for (size_t i = 0; i < data.size(); i++) {
        result[i] = data[i] + key[i % key.size()];
    }

    return result;
}

ByteArray vigenereDecryptBytes(const ByteArray& data, const ByteArray& key) {
    /* Дешифровка шифром Виженера */
    ByteArray result(data.size());

    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = data[i] - key[i % key.size()];
    }
    return result;
}

ByteArray readBinaryFile(const string filePath) {
    /* Считывает файл в вектор байт */
    ifstream inFile(filePath, ios::binary);
    if (!inFile) {
        wcerr << L"Ошибка открытия файла: " << wstring(filePath.begin(), filePath.end()) << endl;
        return {};
    }

    inFile.seekg(0, ios::end);
    size_t size = inFile.tellg();
    inFile.seekg(0, ios::beg);

    ByteArray buffer(size);
    inFile.read(reinterpret_cast<char*>(buffer.data()), size);
    return buffer;
}

void writeBinaryFile(const string filePath, const ByteArray data) {
    /* Записывает вектор байтов в файл */
    ofstream outFile(filePath, ios::binary);
    if (!outFile) {
        wcerr << L"Ошибка открытия файла для записи: " << wstring(filePath.begin(), filePath.end()) << endl;
        return ;
    }

    outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
}

void vigenereEncrypt(const string filePath, const string fileEncryptedPath) {
    ByteArray content = readBinaryFile(filePath);
    if (content.empty()) return;

    ByteArray key = getKeyBytes();
    ByteArray encrypted = vigenereEncryptBytes(content, key);

    writeBinaryFile(fileEncryptedPath, encrypted);
}

void vigenereDecrypt(const string filePath, const string fileDecryptedPath) {
    ByteArray content = readBinaryFile(filePath);
    if (content.empty()) return;

    ByteArray key = getKeyBytes();
    ByteArray decrypted = vigenereDecryptBytes(content, key);

    writeBinaryFile(fileDecryptedPath, decrypted);
}
