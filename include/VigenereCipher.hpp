#pragma once

#include <string>
using namespace std;

extern "C" {
    void vigenereEncrypt(const string filePath, const string fileEncryptedPath);
    void vigenereDecrypt(const string filePath, const string fileDecryptedPath);
}
