#include <array>
#include <cstdint>
#include <stdexcept>
#include <fstream>
#include <iostream>

namespace {

const size_t BLOCK_LEN = 200; // Number ui64 in block

template<typename T, size_t BLOCK_SIZE>
class TExtMatrix {
public:
    TExtMatrix(const char* filename, size_t bytesOffset, size_t rows, size_t cols)
        : File(filename, std::fstream::in | std::fstream::binary)
        , Offset(bytesOffset)
        , Rows(rows)
        , Cols(cols)
    {
    }

    void Read(size_t leftCol, size_t upperRow) {
        if (LeftCacheCol == leftCol && UpperCacheRow == upperRow) {
            return;
        }
        if (leftCol >= Cols || upperRow >= Rows) {
            throw std::out_of_range("Tried to read with starting point out of bounds.");
        }
        LeftCacheCol = leftCol;
        UpperCacheRow = upperRow;
        BufferWidthUsed = std::min(BLOCK_SIZE, Cols - leftCol);
        BufferHeightUsed = std::min(BLOCK_SIZE, Rows - upperRow);

        size_t lineSize = BufferWidthUsed * sizeof(T);
        size_t upperLeftOffset = Offset + (UpperCacheRow * Cols + LeftCacheCol) * sizeof(T);
        char* bufferPtr = (char*)Buffer.data();
        for (size_t row = 0; row < BufferHeightUsed; ++row) {
            File.seekg(upperLeftOffset + row * Cols * sizeof(T), File.beg);
            File.read(bufferPtr, lineSize);
            bufferPtr += lineSize;
        }
    }

    void DebugPrint() {
        auto bufferPtr = Buffer.data();
        for (size_t row = 0; row < BufferHeightUsed; ++row) {
            for (size_t col = 0; col < BufferWidthUsed; ++col, ++bufferPtr) {
                std::cout << (size_t)*bufferPtr << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    void WriteTransponsed(std::ofstream& output) {
        size_t upperLeftOffset = Offset + (LeftCacheCol * Rows + UpperCacheRow) * sizeof(T);
        for (size_t col = 0; col < BufferWidthUsed; ++col) {
            output.seekp(upperLeftOffset + col * Rows * sizeof(T), output.beg);
            for (size_t row = 0; row < BufferHeightUsed; ++row) {
                output.write((char*)(Buffer.data() + col + row * BufferWidthUsed), sizeof(T));
            }
        }
    }

private:
    std::array<T, BLOCK_SIZE * BLOCK_SIZE> Buffer;
    std::fstream File;
    size_t Offset;
    size_t Rows;
    size_t Cols;

    size_t LeftCacheCol = -1;
    size_t UpperCacheRow = -1;
    size_t BufferWidthUsed = 0;
    size_t BufferHeightUsed = 0;
};

void ReadInputShape(const char* path, uint32_t* rows, uint32_t* cols) {
    std::fstream input(path, std::fstream::in | std::fstream::binary);
    input.read((char*)rows, sizeof(*rows));
    input.read((char*)cols, sizeof(*cols));
}

std::ofstream InitOutputFile(const char* path, uint32_t rows, uint32_t cols) {
    std::ofstream output(path, std::fstream::binary);
    output.seekp(2 * sizeof(rows) + rows * cols * sizeof(uint8_t) - 1, output.beg);
    output.put(0);
    output.seekp(0, output.beg);
    output.write((char*)&rows, sizeof(rows));
    output.write((char*)&cols, sizeof(cols));
    return output;
}

} // anonymous namespace

int main() {
    uint32_t rowsCount = 0;
    uint32_t colsCount = 0;
    ReadInputShape("input.bin", &rowsCount, &colsCount);
    TExtMatrix<uint8_t, BLOCK_LEN> matrix("input.bin", 2 * sizeof(rowsCount), rowsCount, colsCount);
    auto outputFile = InitOutputFile("output.bin", colsCount, rowsCount);
    if (colsCount == 1 || rowsCount == 1) {
        return 0;
    }
    for (size_t upperRow = 0; upperRow < rowsCount; upperRow += BLOCK_LEN) {
        for (size_t leftCol = 0; leftCol < colsCount; leftCol += BLOCK_LEN) {
            matrix.Read(leftCol, upperRow);
            matrix.WriteTransponsed(outputFile);
        }
    }
    return 0;
}