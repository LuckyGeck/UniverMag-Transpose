#include <cstdint>
#include <fstream>
#include <iostream>

namespace {
template<typename T>
inline void Write(T val, std::fstream& out) {
    out.write((char*)&val, sizeof(T));
}
} // anonymous namespace

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " [ROWS] [COLS] [FILENAME]" << std::endl;
        return 1;
    }

    uint32_t rows = static_cast<uint32_t>(atoi(argv[1]));
    uint32_t cols = static_cast<uint32_t>(atoi(argv[2]));
    uint8_t numsToGenerate = rows * cols;

    std::fstream out(argv[3], std::fstream::out | std::fstream::binary);
    Write(rows, out);
    Write(cols, out);
    for (uint8_t iter = 1; iter <= numsToGenerate; ++iter) {
        Write(iter, out);
    }
    out.close();
    return 0;
}