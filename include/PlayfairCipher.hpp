#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>

using namespace std;

typedef uint8_t Byte;
typedef vector<Byte> ByteArray;
typedef vector<ByteArray> ByteMatrix;

struct Bigram {
    Byte first;
    Byte second;
};

extern "C" {
    void playfairEncrypt(const string filePath, const string fileEncryptedPath);
    void playfairDecrypt(const string filePath, const string fileDecryptedPath);
}
