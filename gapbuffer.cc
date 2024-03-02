#include <cmath>
#include <string>
#include <string.h>
#include <iostream>

struct GapBuffer {
    char *buffer = nullptr;
    size_t buffer_size = 0;
    size_t gap_size = 16;
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

    GapBuffer(size_t);
    ~GapBuffer();
    void move_gap(int);
    void insert(const std::string&);
    void remove(const size_t);
    void erase();
    void print_debug();
    std::string to_str();
private:
    void resize(size_t);
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

void GapBuffer::resize(size_t size) {
    size_t new_buff_size = 2 * std::max(size, buffer_size);
    char *new_buff = new char[new_buff_size];

    size_t left_size = gap_start - buffer;
    memcpy(new_buff, buffer, left_size);

    size_t right_size = buffer_end() - gap_end();
    memcpy(new_buff + (new_buff_size - right_size), gap_end(), right_size);
    
    gap_start = new_buff + left_size;
    gap_size = new_buff_size - (left_size + right_size);
    buffer_size = new_buff_size;
    delete[] buffer;
    buffer = new_buff;
}

void GapBuffer::insert(const std::string &str) {
    if (str.empty()) {
        return;
    }

    size_t str_size = str.size();
    if (str_size >= gap_size) {
        resize(str_size);
    }
    memcpy(gap_start, str.c_str(), str_size);
    gap_start += str_size;
    gap_size -= str_size;
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
    for (auto it = gap_end(); it < buffer_end(); ++it) {
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
    buff.insert("(ABCDEFGH)");
    buff.move_gap(-2);
    buff.insert("abc");
    buff.print_debug();
    std::cout << "String: " << buff.to_str() << "\nLength:" << buff.length() << std::endl;
    buff.erase();
    buff.print_debug();
}
