#include "MatrixCipher.hpp"

void fillSpiral(const ByteArray fileContent, MatrixArray& mtrxs) {
    size_t idx = 0;
    while (idx < fileContent.size()) {
        Matrix spiralMatrix(11, ByteArray(11, 0));

        int top = 0, bottom = 10, left = 0, right = 10;

        while (top <= bottom && left <= right && idx < fileContent.size()) {
            for (int i = left; i <= right && idx < fileContent.size(); i++)
                spiralMatrix[top][i] = fileContent[idx++];
            top++;

            for (int i = top; i <= bottom && idx < fileContent.size(); i++)
                spiralMatrix[i][right] = fileContent[idx++];
            right--;

            if (top <= bottom) {
                for (int i = right; i >= left && idx < fileContent.size(); i--)
                    spiralMatrix[bottom][i] = fileContent[idx++];
                bottom--;
            }

            if (left <= right) {
                for (int i = bottom; i >= top && idx < fileContent.size(); i--)
                    spiralMatrix[i][left] = fileContent[idx++];
                left++;
            }
        }

        mtrxs.push_back(spiralMatrix);
    }
}

void unrollSpiral(const ByteArray fileContent, MatrixArray& mtrxs) {
    size_t idx = 0;
    while (idx < fileContent.size()) {
        Matrix spiralMatrix(11, ByteArray(11, 0));

        for (size_t i = 0; i < 11 && idx < fileContent.size(); i++) {
            for (size_t j = 0; j < 11 && idx < fileContent.size(); j++) {
                spiralMatrix[i][j] = fileContent[idx++];
            }
        }

        Matrix matrix(11, ByteArray(11, 0));

        int top = 0, bottom = 10, left = 0, right = 10, readIdx = 0;

        while (top <= bottom && left <= right && readIdx < 121) {
            for (int i = left; i <= right && readIdx < 121; i++) {
                matrix[readIdx / 11][readIdx % 11] = spiralMatrix[top][i], readIdx++;
            }
            top++;

            for (int i = top; i <= bottom && readIdx < 121; i++) {
                matrix[readIdx / 11][readIdx % 11] = spiralMatrix[i][right], readIdx++;
            }
            right--;

            for (int i = right; i >= left && readIdx < 121; i--) {
                matrix[readIdx / 11][readIdx % 11] = spiralMatrix[bottom][i], readIdx++;
            }
            bottom--;

            for (int i = bottom; i >= top && readIdx < 121; i--) {
                matrix[readIdx / 11][readIdx % 11] = spiralMatrix[i][left], readIdx++;
            }
            left++;
        }

        mtrxs.push_back(matrix);
    }
}

ByteArray matrixsToBytes(const MatrixArray mtrxs) {
    ByteArray result;
    for (const auto& matrix : mtrxs) {
        for (const auto& row : matrix) {
            for (const auto& elm : row) {
                result.push_back(elm);
            }
        }
    }
    return result;
}

void trimRight(ByteArray& data) {
    while (!data.empty() && data.back() == 0) {
        data.pop_back();
    }
}

void fillCryptoFile(const string filePath, const MatrixArray matrixs) {
    ByteArray output = matrixsToBytes(matrixs);
    trimRight(output);

    ofstream outFile(filePath, ios::binary);
    outFile.write(reinterpret_cast<const char*>(output.data()), output.size());
}

void matrixEncrypt(const string filePath, const string fileEncryptedPath) {
    ifstream inFile(filePath, ios::binary);
    if (!inFile) {
        cerr << "[ ERROR ] Ошибка открытия файла: " << string(filePath.begin(), filePath.end()) << endl;
        return;
    }

    inFile.seekg(0, ios::end);
    size_t size = inFile.tellg();
    inFile.seekg(0, ios::beg);

    ByteArray fileContent(size);
    inFile.read(reinterpret_cast<char*>(fileContent.data()), size);

    MatrixArray spiralMatrixs;
    fillSpiral(fileContent, spiralMatrixs);
    fillCryptoFile(fileEncryptedPath, spiralMatrixs);
}

void matrixDecrypt(const string filePath, const string fileDecryptedPath) {
    ifstream inFile(filePath, ios::binary);

    if (!inFile) {
        cerr << "[ ERROR ] Ошибка открытия файла: " << string(filePath.begin(), filePath.end()) << endl;
        return;
    }

    inFile.seekg(0, ios::end);
    size_t size = inFile.tellg();
    inFile.seekg(0, ios::beg);

    ByteArray fileContent(size);
    inFile.read(reinterpret_cast<char*>(fileContent.data()), size);

    MatrixArray matrixs;
    unrollSpiral(fileContent, matrixs);
    fillCryptoFile(fileDecryptedPath, matrixs);
}
