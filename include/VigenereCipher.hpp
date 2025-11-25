#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <limits>

using namespace std;

typedef uint8_t Byte;
typedef vector<Byte> ByteArray;

extern "C" {
    ByteArray vigenereEncrypt(
        const string& filePath,
        const string& fileEncryptedPath,
        const string& userCryptoText
    );
    ByteArray vigenereDecrypt(
        const string& filePath,
        const string& fileDecryptedPath,
        const string& userCryptoText
    );
}
