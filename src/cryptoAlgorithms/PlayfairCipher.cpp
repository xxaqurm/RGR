#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>

#include "PlayfairCipher.hpp"

using namespace std;

typedef uint8_t Byte;
typedef vector<Byte> ByteArray;
typedef vector<ByteArray> ByteMatrix;

struct Bigram {
    Byte first;
    Byte second;
};

ByteArray getKeyBytesPlayfair() {
    /* Преобразует строку-ключ в вектор байтов */
    wstring key;
    while (key.empty()) {
        wcout << L"Введите ключевое слово на латинице для шифрования/дешифрования: ";
        getline(wcin, key);
    }

    ByteArray result;
    result.reserve(key.size());
    for (wchar_t ch : key) {
        result.push_back(static_cast<Byte>(ch & 0xFF));
    }
    return result;
}

ByteArray readBinaryFilePlayfair(const string filePath) {
    /* Считывает файл в вектор байтов */
    ifstream inFile(filePath, ios::binary);
    if(!inFile) {
        wcerr << L"Ошибка открытия файла: " << wstring(filePath.begin(), filePath.end());
        return {};
    }

    inFile.seekg(0, ios::end);
    size_t size = inFile.tellg();
    inFile.seekg(0, ios::beg);
    
    ByteArray content(size);
    inFile.read(reinterpret_cast<char*>(content.data()), size);
    return content;
}

void writeBinaryFilePlayfair(const string filePath, const ByteArray data){
    /* Записывает вектор байтов в файл */
    ofstream outFile(filePath, ios::binary);
    if (!outFile) {
        wcerr << L"Ошибка открытия файла для записи: " << wstring(filePath.begin(), filePath.end());
        return ;
    }

    outFile.write(reinterpret_cast<const  char*>(data.data()), data.size());
}

bool findMatch(const ByteArray bytes, const Byte b) {
    /* Ищет совпадения в bytes с b */
    for (auto& elm : bytes) {
        if (elm == b) return true;
    }
    return false;
}

ByteMatrix genKeyMatrix(const ByteArray key) {
    ByteArray usedBytes;
    for (Byte b : key) {
        if (!findMatch(usedBytes, b)) {
            usedBytes.push_back(b);
        }
    }

    for (int b = 0; b < 256; b++) {
        if (!findMatch(usedBytes, static_cast<Byte>(b))) {
            usedBytes.push_back(static_cast<Byte>(b));
        }
    }

    ByteMatrix matrix(16, ByteArray(16, 0));
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            matrix[i][j] = usedBytes[i * 16 + j];
        }
    }
    return matrix;
}

vector<Bigram> splitToBigrams(const ByteArray content) {
    vector<Bigram> bigrams;
    bigrams.reserve((content.size() + 1) / 2);

    size_t idx = 0;
    while (idx < content.size()) {
        Byte first = content[idx++];
        Byte second = (idx < content.size()) ? content[idx++] : 0xFF;
        bigrams.push_back({first, second});
    }
    return bigrams;
}

ByteArray addSplitterToContent(const ByteArray content) {
    Byte splitter = 0xFF;
    if (content.empty()) return {};

    ByteArray result;
    result.reserve(content.size() + content.size() / 2 + 2);

    for (size_t i = 0; i + 1 < content.size(); i++) {
        result.push_back(content[i]);
        if (content[i] == content[i + 1]) {
            result.push_back(splitter);
        }
    }
    result.push_back(content.back());

    if (result.size() % 2)  {
        result.push_back(splitter);
    }

    return result;
}

ByteArray removeSplitterFromContent(const ByteArray content) {
    Byte splitter = 0xFF;
    ByteArray result;
    result.reserve(content.size());
    for (Byte b : content) {
        if (b != splitter) {
            result.push_back(b);
        }
    }
    return result;
}

pair<int, int> findIdxInKey(const ByteMatrix key, const Byte b) {
    for (size_t i = 0; i < key.size(); i++) {
        for (size_t j = 0; j < key.size(); j++) {
            if (key[i][j] == b) {
                return {(int)i, (int)j};
            }
        }
    }

    return {-1, -1};
}

ByteArray playfairEncryptBytes(const ByteArray content, const ByteMatrix key) {
    /* Шифрование */
    ByteArray withSplitter = addSplitterToContent(content);
    vector<Bigram> bigrams = splitToBigrams(withSplitter);
    vector<Bigram> encryptBigrams(bigrams.size());

    for (size_t i = 0; i < bigrams.size(); i++) {
        pair<int,  int> posFirstByte = findIdxInKey(key, bigrams[i].first);
        pair<int, int> posSecondByte = findIdxInKey(key, bigrams[i].second);

        Byte firstElm = 0;
        Byte secondElm = 0;
        if (posFirstByte.first != posSecondByte.first && posFirstByte.second != posSecondByte.second) {
            // Берем противоположные углы прямоугольника
            firstElm = key[posFirstByte.first][posSecondByte.second];
            secondElm = key[posSecondByte.first][posFirstByte.second];
        } else if (posFirstByte.first != posSecondByte.first && posFirstByte.second == posSecondByte.second) {
            // Берем нижние элементы столбца
            firstElm = (posFirstByte.first + 1 < (int)key.size()) ? key[posFirstByte.first + 1][posFirstByte.second] : key[0][posFirstByte.second];
            secondElm = (posSecondByte.first + 1 < (int)key.size()) ? key[posSecondByte.first + 1][posSecondByte.second] : key[0][posSecondByte.second];
        } else if (posFirstByte.first == posSecondByte.first && posFirstByte.second != posSecondByte.second) {
            // Берем правые элементы строки
            firstElm = (posFirstByte.second + 1 < (int)key.size()) ? key[posFirstByte.first][posFirstByte.second + 1] : key[posFirstByte.first][0];
            secondElm = (posSecondByte.second + 1 < (int)key.size()) ? key[posSecondByte.first][posSecondByte.second + 1] : key[posSecondByte.first][0];
        } else {  // один и тот же элемент
            wcerr << L"Error: something wrong (playfairEncryptBytes function)." << endl;
            return {};
        }
        encryptBigrams[i] =  {firstElm, secondElm};
    }

    ByteArray result;
    result.reserve(encryptBigrams.size() * 2);
    for (auto& bg : encryptBigrams) {
        result.push_back(bg.first);
        result.push_back(bg.second);
    }
    return result;
}

ByteArray playfairDecryptBytes(const ByteArray content, const ByteMatrix key) {
    /* Дешифрование */
    vector<Bigram> bigrams = splitToBigrams(content);
    vector<Bigram> decryptBigrams(bigrams.size());

    for (size_t i = 0; i < bigrams.size(); i++) {
        pair<int,  int> posFirstByte = findIdxInKey(key, bigrams[i].first);
        pair<int, int> posSecondByte = findIdxInKey(key, bigrams[i].second);

        Byte firstElm = 0;
        Byte secondElm = 0;
        if (posFirstByte.first != posSecondByte.first && posFirstByte.second != posSecondByte.second) {
            // Берем противоположные углы прямоугольника
            firstElm = key[posFirstByte.first][posSecondByte.second];
            secondElm = key[posSecondByte.first][posFirstByte.second];
        } else if (posFirstByte.first != posSecondByte.first && posFirstByte.second == posSecondByte.second) {
            // Берем верхние элементы столбца
            firstElm = (posFirstByte.first - 1 >= 0) ? key[posFirstByte.first - 1][posFirstByte.second] : key[key.size() - 1][posFirstByte.second];
            secondElm = (posSecondByte.first - 1 >= 0) ? key[posSecondByte.first - 1][posSecondByte.second] : key[key.size() - 1][posSecondByte.second];
        } else if (posFirstByte.first == posSecondByte.first && posFirstByte.second != posSecondByte.second) {
            // Берем левые элементы строки
            firstElm = (posFirstByte.second - 1 >= 0) ? key[posFirstByte.first][posFirstByte.second - 1] : key[posFirstByte.first][key.size() - 1];
            secondElm = (posSecondByte.second - 1 >= 0) ? key[posSecondByte.first][posSecondByte.second - 1] : key[posSecondByte.first][key.size() - 1];
        } else {  // один и тот же элемент
            wcerr << L"Error: something wrong (playfairEncryptBytes function)." << endl;
            return {};
        }
        decryptBigrams[i] =  {firstElm, secondElm};
    }

    ByteArray result;
    result.reserve(decryptBigrams.size() * 2);
    for (auto& bg : decryptBigrams) {
        result.push_back(bg.first);
        result.push_back(bg.second);
    }
    return removeSplitterFromContent(result);
}

void playfairEncrypt(const string filePath, const string fileEncryptedPath) {
    ByteArray fileContent = readBinaryFilePlayfair(filePath);
    if (fileContent.empty()) return ;

    ByteArray key = getKeyBytesPlayfair();
    ByteMatrix keyMatrix = genKeyMatrix(key);
    ByteArray encryptedContent = playfairEncryptBytes(fileContent, keyMatrix);

    writeBinaryFilePlayfair(fileEncryptedPath, encryptedContent);
}

void playfairDecrypt(const string filePath, const string fileDecryptedPath) {
    ByteArray fileContent = readBinaryFilePlayfair(filePath);
    if (fileContent.empty()) return ;

    ByteArray key = getKeyBytesPlayfair();
    ByteMatrix keyMatrix = genKeyMatrix(key);
    ByteArray decryptedContent = playfairDecryptBytes(fileContent, keyMatrix);

    writeBinaryFilePlayfair(fileDecryptedPath, decryptedContent);
}
