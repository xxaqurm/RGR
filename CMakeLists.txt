cmake_minimum_required(VERSION 3.10)
project(comprehensive_data_protection_system)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Установка путей вывода в папке build/
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Заголовочные файлы
include_directories(include)

# === Библиотека MatrixCipher ===
add_library(MatrixCipher SHARED src/cryptoAlgorithms/MatrixCipher.cpp)
target_include_directories(MatrixCipher PUBLIC include)

# === Библиотека PlayfairCipher ===
add_library(PlayfairCipher SHARED src/cryptoAlgorithms/PlayfairCipher.cpp)
target_include_directories(PlayfairCipher PUBLIC include)

# === Библиотека VigenereCipher ===
add_library(VigenereCipher SHARED src/cryptoAlgorithms/VigenereCipher.cpp)
target_include_directories(VigenereCipher PUBLIC include)

# === Главный исполняемый файл ===
add_executable(comprehensive_data_protection_system main.cpp src/utils/utils.cpp)

# Линковка с библиотеками
target_link_libraries(comprehensive_data_protection_system
    MatrixCipher
    PlayfairCipher
    VigenereCipher
)