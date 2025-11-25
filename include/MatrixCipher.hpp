#pragma once

#include <fstream>
#include <vector>
#include <iostream>
#include <cstdint>
#include <limits>

using namespace std;

typedef uint8_t Byte;
typedef vector<Byte> ByteArray;
typedef vector<ByteArray> Matrix;
typedef vector<Matrix> MatrixArray;

extern "C" {
    ByteArray matrixEncrypt(
        const string& filePath,
        const string& fileEncryptedPath,
        const string& userCryptoText
    );
    ByteArray matrixDecrypt(
        const string& filePath,
        const string& fileDecryptedPath,
        const string& userCryptoText
    );
}
