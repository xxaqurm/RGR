#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

using namespace std;

typedef uint8_t Byte;
typedef vector<Byte> ByteArray;

extern "C" {
    void vigenereEncrypt(const string filePath, const string fileEncryptedPath);
    void vigenereDecrypt(const string filePath, const string fileDecryptedPath);
}
