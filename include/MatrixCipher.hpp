#pragma once

#include <fstream>
#include <vector>
#include <iostream>
#include <cstdint>

using namespace std;

typedef uint8_t Byte;
typedef vector<Byte> ByteArray;
typedef vector<ByteArray> Matrix;
typedef vector<Matrix> MatrixArray;

extern "C" {
    void matrixEncrypt(const string filePath, const string fileEncryptedPath);
    void matrixDecrypt(const string filePath, const string fileDecryptedPath);
}
