#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>
#include <limits>

using namespace std;

typedef uint8_t Byte;
typedef vector<Byte> ByteArray;
typedef vector<ByteArray> ByteMatrix;

struct Bigram {
    Byte first;
    Byte second;
};

extern "C" {
    ByteArray playfairEncrypt(
        const string& filePath, 
        const string& fileEncryptedPath, 
        const string& userCryptoText 
    );
    ByteArray playfairDecrypt(
        const string& filePath, 
        const string& fileDecryptedPath, 
        const string& userCryptoText 
    );
}
