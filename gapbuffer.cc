#include <cmath>
#include <string>
#include <string.h>
#include <iostream>

struct GapBuffer {
private:
    char *buffer = nullptr;
    size_t buffer_size = 0;
    size_t gap_size = 0;
    char *gap_start = nullptr;

    void resize(size_t);

    inline char *gap_end() {
        return gap_start + gap_size;
    }

    inline char *buffer_end() {
        return buffer + buffer_size;
    }
public:
    GapBuffer(size_t);
    ~GapBuffer();
    void move_gap(int); // move amount
    void move_gap_to(size_t); // move to specific index
    void insert(const std::string&);
    void insert(const char*, size_t);
    void insert(const char*);
    char at(size_t);
    char operator[](size_t idx) { return at(idx); }
    size_t remove(const size_t);
    void erase();
    void print_debug();
    std::string to_str();

    size_t length() {
        return buffer_size - gap_size;
    }

    size_t size() {
        return length();
    }

    bool empty() {
        return length() == 0;
    }
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

// move gap to offset relatively to the current position
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

// move gap to specific offset position
void GapBuffer::move_gap_to(size_t offset) {
    int amount = 0;
    char *new_pos = buffer + offset;
    if (new_pos < gap_start) {
        int amount = gap_start - new_pos;
        move_gap(-amount);
    }

    else if (new_pos > gap_start) {
        int amount = new_pos - gap_start;
        move_gap(amount);
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

// insert N bytes
void GapBuffer::insert(const char *bytes, size_t size) {
    if (!size) return;
    if (size >= gap_size) {
        resize(size);
    }
    memcpy(gap_start, bytes, size);
    gap_start += size;
    gap_size -= size;
}

// insert NULL terminated C-string
void GapBuffer::insert(const char *c_str) {
    insert(c_str, strlen(c_str));
}

// insert C++ dynamic string
void GapBuffer::insert(const std::string &str) {
    insert(str.c_str(), str.size());
}

size_t GapBuffer::remove(const size_t amount) {
    if (gap_start - amount >= buffer) {
        gap_size += amount;
        gap_start -= amount;

        return amount;
    }
    size_t removed = (gap_start - buffer);
    gap_size += removed;
    gap_start = buffer;

    return removed;
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

char GapBuffer::at(size_t idx) {
    size_t gap_offset = gap_start - buffer;
    if (idx < gap_offset) {
        return *(buffer + idx);
    }

    size_t delta = idx - gap_offset;
    return *(gap_end() + delta);
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
