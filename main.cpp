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
} // anonymous namespace

int main() {
    TExtMatrix<uint8_t, 10> matrix("input.bin", 2 * sizeof(uint32_t), 10, 10);
    matrix.Read(9, 0);
    matrix.DebugPrint();
}