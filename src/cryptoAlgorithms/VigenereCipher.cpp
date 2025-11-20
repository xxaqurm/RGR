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

ByteArray readBinaryFile(const string filePath) {
    ifstream inFile(filePath, ios::binary);
    if (!inFile) {
        cerr << "Ошибка открытия файла: " << string(filePath.begin(), filePath.end()) << endl;
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
    ofstream outFile(filePath, ios::binary);
    if (!outFile) {
        cerr << "Ошибка открытия файла для записи: " << string(filePath.begin(), filePath.end()) << endl;
        return ;
    }

    outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
}

void vigenereEncrypt(const string filePath, const string fileEncryptedPath) {
    ByteArray content = readBinaryFile(filePath);
    if (content.empty()) {
        return;
    }

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
