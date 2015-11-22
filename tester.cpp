#include <cstdint>
#include <fstream>
#include <iostream>

void ReadInputShape(const char* path, uint32_t* rows, uint32_t* cols) {
    std::fstream input(path, std::fstream::in | std::fstream::binary);
    input.read((char*)rows, sizeof(*rows));
    input.read((char*)cols, sizeof(*cols));
}

void PrintFile(const char* path) {
    uint32_t rows = 0;
    uint32_t cols = 0;
    std::fstream input(path, std::fstream::in | std::fstream::binary);
    input.read((char*)&rows, sizeof(rows));
    input.read((char*)&cols, sizeof(cols));
    uint8_t data = 0;
    char* dataPtr = (char*)&data;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            input.read(dataPtr, sizeof(data));
            std::cout << (size_t)data << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [FILENAME]" << std::endl;
        return 1;
    }

    uint32_t rowsCount = 0;
    uint32_t colsCount = 0;
    ReadInputShape(argv[1], &rowsCount, &colsCount);

    std::fstream in(argv[1], std::fstream::in | std::fstream::binary);
    in.seekg(2 * sizeof(rowsCount), in.beg);
    uint8_t data = 0;
    char* dataPtr = (char*)&data;
    for (size_t row = 0; row < rowsCount; ++row) {
        for (size_t col = 0; col < colsCount; ++col) {
            in.read(dataPtr, sizeof(data));
            if (data != (col * rowsCount + row + 1) % 256) {
                PrintFile(argv[1]);
                return 1;
            }
        }
    }
    return 0;
}