#pragma once

#include <string>
using namespace std;

extern "C" {
    void matrixEncrypt(const string filePath, const string fileEncryptedPath);
    void matrixDecrypt(const string filePath, const string fileDecryptedPath);
}
