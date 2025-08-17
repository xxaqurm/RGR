#pragma once

#include <string>
using namespace std;

extern "C" {
    void playfairEncrypt(const string filePath, const string fileEncryptedPath);
    void playfairDecrypt(const string filePath, const string fileDecryptedPath);
}
