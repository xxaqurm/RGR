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

ByteArray playfairEncrypt(const string& filePath, const string& fileEncryptedPath, const string& userCryptoText) {
    ByteArray fileContent;
    if (filePath == "default" && fileEncryptedPath == "default" && userCryptoText != "default") {
        for (const uint8_t b : userCryptoText) {
            fileContent.push_back(b);
        }
    } else {
        ifstream inFile(filePath, ios::binary);
        if (!inFile) {
            cerr << "[ ERROR ] Ошибка открытия файла: " << filePath << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return ByteArray{};
        }

        inFile.seekg(0, ios::end);
        size_t size_ = inFile.tellg();
        inFile.seekg(0, ios::beg);

        fileContent.resize(size_);
        inFile.read(reinterpret_cast<char*>(fileContent.data()), size_);
    }

    ByteArray key = getKeyBytesPlayfair();
    ByteMatrix keyMatrix = genKeyMatrix(key);
    ByteArray output = playfairEncryptBytes(fileContent, keyMatrix);

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

ByteArray playfairDecrypt(const string& filePath, const string& fileDecryptedPath, const string& userCryptoText) {
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
            cerr << "[ ERROR ] Ошибка открытия файла: " << filePath << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return ByteArray{};
        }

        inFile.seekg(0, ios::end);
        size_t size_ = inFile.tellg();
        inFile.seekg(0, ios::beg);

        fileContent.resize(size_);
        inFile.read(reinterpret_cast<char*>(fileContent.data()), size_);
    }

    ByteArray key = getKeyBytesPlayfair();
    ByteMatrix keyMatrix = genKeyMatrix(key);
    ByteArray output = playfairDecryptBytes(fileContent, keyMatrix);

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
