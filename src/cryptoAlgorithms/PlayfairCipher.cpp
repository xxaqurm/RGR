#include "PlayfairCipher.hpp"

ByteArray getKeyBytesPlayfair() {
    string key;
    while (key.empty()) {
        cout << "Введите ключевое слово: ";
        getline(cin, key);
    }

    ByteArray result;
    result.reserve(key.size());
    for (unsigned char ch : key) {
        result.push_back(static_cast<Byte>(ch));
    }
    return result;
}

ByteArray readBinaryFilePlayfair(const string& filePath) {
    ifstream inFile(filePath, ios::binary);
    if (!inFile) {
        cerr << "Ошибка открытия файла для чтения: " << filePath << endl;
        return {};
    }

    inFile.seekg(0, ios::end);
    streamoff s = inFile.tellg();
    if (s < 0) {
        cerr << "Не удалось определить размер файла: " << filePath << endl;
        return {};
    }
    size_t size = static_cast<size_t>(s);
    inFile.seekg(0, ios::beg);

    ByteArray content(size);
    if (size > 0)
        inFile.read(reinterpret_cast<char*>(content.data()), static_cast<streamsize>(size));
    return content;
}

void writeBinaryFilePlayfair(const string& filePath, const ByteArray& data) {
    ofstream outFile(filePath, ios::binary);
    if (!outFile) {
        cerr << "Ошибка открытия файла для записи: " << filePath << endl;
        return;
    }

    if (!data.empty())
        outFile.write(reinterpret_cast<const char*>(data.data()), static_cast<streamsize>(data.size()));
}

bool findMatch(const ByteArray& bytes, Byte b) {
    for (Byte elm : bytes) {
        if (elm == b) return true;
    }
    return false;
}

ByteMatrix genKeyMatrix(const ByteArray& key) {
    ByteArray usedBytes;
    usedBytes.reserve(256);

    for (Byte b : key) {
        if (!findMatch(usedBytes, b)) {
            usedBytes.push_back(b);
        }
    }

    for (int b = 0; b < 256; ++b) {
        Byte bb = static_cast<Byte>(b);
        if (!findMatch(usedBytes, bb)) {
            usedBytes.push_back(bb);
        }
    }

    ByteMatrix matrix(16, ByteArray(16, 0));
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            matrix[i][j] = usedBytes[i * 16 + j];
        }
    }

    return matrix;
}

vector<Bigram> splitToBigrams(const ByteArray& content) {
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

ByteArray addSplitterToContent(const ByteArray& content) {
    const Byte splitter = 0xFF;
    if (content.empty()) return {};

    ByteArray result;
    result.reserve(content.size() * 2);

    size_t i = 0;
    while (i < content.size()) {
        Byte a = content[i];
        if (i + 1 < content.size()) {
            Byte b = content[i + 1];
            if (a == b) {
                result.push_back(a);
                result.push_back(splitter);
                ++i;
            } else {
                result.push_back(a);
                result.push_back(b);
                i += 2;
            }
        } else {
            result.push_back(a);
            result.push_back(splitter);
            ++i;
        }
    }

    return result;
}

ByteArray removeSplitterFromContent(const ByteArray& content) {
    const Byte splitter = 0xFF;
    ByteArray result;
    result.reserve(content.size());
    for (Byte b : content) {
        if (b != splitter) result.push_back(b);
    }
    return result;
}

pair<int,int> findIdxInKey(const ByteMatrix& key, Byte b) {
    for (size_t i = 0; i < key.size(); ++i) {
        for (size_t j = 0; j < key[i].size(); ++j) {
            if (key[i][j] == b) return {static_cast<int>(i), static_cast<int>(j)};
        }
    }
    return {-1, -1};
}

ByteArray playfairEncryptBytes(const ByteArray& content, const ByteMatrix& key) {
    ByteArray withSplitter = addSplitterToContent(content);
    vector<Bigram> bigrams = splitToBigrams(withSplitter);
    vector<Bigram> encryptBigrams(bigrams.size());

    for (size_t i = 0; i < bigrams.size(); ++i) {
        auto posFirst = findIdxInKey(key, bigrams[i].first);
        auto posSecond = findIdxInKey(key, bigrams[i].second);

        if (posFirst.first < 0 || posSecond.first < 0) {
            cerr << "Ошибка: байт не найден в матрице ключа при шифровании." << endl;
            return {};
        }

        Byte firstElm = 0;
        Byte secondElm = 0;

        if (posFirst.first != posSecond.first && posFirst.second != posSecond.second) { // противоположные углы
            firstElm  = key[posFirst.first][posSecond.second];
            secondElm = key[posSecond.first][posFirst.second];
        } else if (posFirst.first != posSecond.first && posFirst.second == posSecond.second) { // одинаковый столбец
            size_t n = key.size();
            firstElm  = key[(posFirst.first + 1) % n][posFirst.second];
            secondElm = key[(posSecond.first + 1) % n][posSecond.second];
        } else if (posFirst.first == posSecond.first && posFirst.second != posSecond.second) { // одинаковая строка
            size_t m = key[posFirst.first].size();
            firstElm  = key[posFirst.first][(posFirst.second + 1) % m];
            secondElm = key[posSecond.first][(posSecond.second + 1) % m];
        } else { // один и тот же элемент
            size_t n = key.size();
            firstElm  = key[(posFirst.first + 1) % n][posFirst.second];
            secondElm = key[(posSecond.first + 1) % n][posSecond.second];
        }

        encryptBigrams[i] = { firstElm, secondElm };
    }

    ByteArray result;
    result.reserve(encryptBigrams.size() * 2);
    for (const auto& bg : encryptBigrams) {
        result.push_back(bg.first);
        result.push_back(bg.second);
    }
    return result;
}

ByteArray playfairDecryptBytes(const ByteArray& content, const ByteMatrix& key) {
    vector<Bigram> bigrams = splitToBigrams(content);
    vector<Bigram> decryptBigrams(bigrams.size());

    for (size_t i = 0; i < bigrams.size(); ++i) {
        auto posFirst = findIdxInKey(key, bigrams[i].first);
        auto posSecond = findIdxInKey(key, bigrams[i].second);

        if (posFirst.first < 0 || posSecond.first < 0) {
            cerr << "Ошибка: байт не найден в матрице ключа при дешифровании." << endl;
            return {};
        }

        Byte firstElm = 0;
        Byte secondElm = 0;

        if (posFirst.first != posSecond.first && posFirst.second != posSecond.second) { // противоположные углы
            firstElm  = key[posFirst.first][posSecond.second];
            secondElm = key[posSecond.first][posFirst.second];
        } else if (posFirst.first != posSecond.first && posFirst.second == posSecond.second) { // одинаковый столбец
            size_t n = key.size();
            firstElm  = key[(posFirst.first + n - 1) % n][posFirst.second];
            secondElm = key[(posSecond.first + n - 1) % n][posSecond.second];
        } else if (posFirst.first == posSecond.first && posFirst.second != posSecond.second) { // одинаковая строка
            size_t m = key[posFirst.first].size();
            firstElm  = key[posFirst.first][(posFirst.second + m - 1) % m];
            secondElm = key[posSecond.first][(posSecond.second + m - 1) % m];
        } else { // один и тот же элемент
            size_t n = key.size();
            firstElm  = key[(posFirst.first + n - 1) % n][posFirst.second];
            secondElm = key[(posSecond.first + n - 1) % n][posSecond.second];
        }

        decryptBigrams[i] = { firstElm, secondElm };
    }

    ByteArray result;
    result.reserve(decryptBigrams.size() * 2);
    for (const auto& bg : decryptBigrams) {
        result.push_back(bg.first);
        result.push_back(bg.second);
    }
    return removeSplitterFromContent(result);
}

void playfairEncrypt(const string filePath, const string fileEncryptedPath) {
    ByteArray fileContent = readBinaryFilePlayfair(filePath);
    if (fileContent.empty()) {
        cerr << "Пустой файл или ошибка чтения: " << filePath << endl;
        return;
    }

    ByteArray key = getKeyBytesPlayfair();
    ByteMatrix keyMatrix = genKeyMatrix(key);
    ByteArray encryptedContent = playfairEncryptBytes(fileContent, keyMatrix);

    writeBinaryFilePlayfair(fileEncryptedPath, encryptedContent);
}

void playfairDecrypt(const string filePath, const string fileDecryptedPath) {
    ByteArray fileContent = readBinaryFilePlayfair(filePath);
    if (fileContent.empty()) {
        cerr << "Пустой файл или ошибка чтения: " << filePath << endl;
        return;
    }

    ByteArray key = getKeyBytesPlayfair();
    ByteMatrix keyMatrix = genKeyMatrix(key);
    ByteArray decryptedContent = playfairDecryptBytes(fileContent, keyMatrix);

    writeBinaryFilePlayfair(fileDecryptedPath, decryptedContent);
}