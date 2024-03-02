#include <list>
#include <cmath>
#include <string>
#include <string.h>
#include <iostream>
#include <cassert>

#define GAP_SIZE 16

struct GapBuffer {
    char *buffer = nullptr;
    size_t buffer_size = 0;
    size_t gap_size = GAP_SIZE;
    char *gap_start = nullptr;

    inline char *gap_end() {
        return gap_start + gap_size;
    }

    inline char *buffer_end() {
        return buffer + buffer_size;
    }

    size_t length() {
        return buffer_size - gap_size;
    }

    bool empty() {
        return length() == 0;
    }

    GapBuffer(size_t );
    ~GapBuffer();
    void move_gap(int);
    void insert(const std::string&);
    void remove(const size_t);
    void erase();
    void print_debug();
    std::string to_str();
};

GapBuffer::GapBuffer(size_t nbytes) {
    buffer = new char[nbytes];
    buffer_size = nbytes;
    gap_start = buffer;
    gap_size = buffer_size;
}

GapBuffer::~GapBuffer() {
    delete[] buffer;
}

void GapBuffer::move_gap(int amount) {
    if (!amount) return;

    size_t size = std::abs(amount);
    if (amount < 0) {
        auto src = gap_start - size;
        auto dst = gap_end() - size;
        memcpy(dst, src, size);
        gap_start = src;
    } else {
        auto src = gap_end();
        auto dst = gap_start;
        memcpy(dst, src, size);
        gap_start = dst + size;
    }
}

void GapBuffer::remove(const size_t amount) {
    if (gap_start - amount >= buffer) {
        gap_start -= amount;
        gap_size += amount;
    }
}

void GapBuffer::erase() {
    gap_start = buffer;
    gap_size = buffer_size;
}

void GapBuffer::insert(const std::string &str) {
    if (str.empty()) {
        return;
    }

    size_t str_size = str.size();
    // TODO: extend gap and buffer when str_size >= gap_size
    assert(str_size < gap_size);
    memcpy(gap_start, str.c_str(), str_size);
    gap_start += str_size;
    gap_size -= str_size;
}

std::string GapBuffer::to_str() {
    std::string str;
    str.append(buffer, gap_start);
    str.append(gap_end(), buffer_end());

    return str;
}

void GapBuffer::print_debug() {
    for (auto it = buffer; it < gap_start; ++it) {
        std::cout << *it;
    }
    for (int i = 0; i < gap_size; ++i) {
        std::cout << "_";
    }
    for (auto it = gap_start + gap_size; it < buffer + buffer_size; ++it) {
        std::cout << *it;
    }
    std::cout << std::endl;
}

int main() {
    GapBuffer buff(8);
    buff.insert("Hell");
    buff.print_debug();
    buff.insert("Oh");
    buff.print_debug();
    buff.move_gap(-3);
    buff.print_debug();
    buff.insert("P");
    buff.print_debug();
    buff.move_gap(1);
    buff.print_debug();
    buff.remove(2);
    buff.insert("l");
    buff.print_debug();
    std::cout << "String: " << buff.to_str() << "\nLength:" << buff.length() << std::endl;
    buff.erase();
    buff.print_debug();
}
