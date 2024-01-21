#pragma once
#include "utf8.hh"
#include <iostream>
#include <exception>

int codepointSize(char byte) {
    if ((byte & 0x80) == 0) // 0xxxxxxx
        return 1;
    else if ((byte & 0xE0) == 0xC0) // 110xxxxx
        return 2;
    else if ((byte & 0xF0) == 0xE0) // 1110xxxx
        return 3;
    else if ((byte & 0xF8) == 0xF0) // 11110xxx
        return 4;

    // idk what to do if codepoint size is unknown
    return -1;
}

size_t columnLen(const std::string& str) {
    size_t len = 0;
    size_t i = 0;

    while(i < str.length()) {
        char c = str[i];
        if (c == '\r') {
            i++;
            continue;
        }

        i += codepointSize(c);
        len++;
    }

    return len;
}

void removeChar(std::string& str, int pos)
{
    size_t i = 0; // index relative to string bytes
    size_t j = 0; // index relative to "printable" unicode length

    while(i <= str.length()) {
        size_t cSize = codepointSize(str[i]);
        if (cSize == -1) {
            throw std::runtime_error("Invalid UTF-8 character");
        }

        if (j == pos) {
            str.erase(i, cSize);
            break;
        }

        i += cSize;
        j++;
    }
}

std::string substr(const std::string& str, int pos, int n) {
    size_t i = 0; // index relative to string bytes
    size_t j = 0; // index relative to "printable" unicode length

    bool isSet = false;
    int start = 0;
    int end = 0;

    while(i <= str.length()) {
        size_t cSize = codepointSize(str[i]);
        if (cSize == -1) {
            throw std::runtime_error("Invalid UTF-8 character");
        }

        if (j == pos) {
            isSet = true;
            start = i;
        }

        if (j == start + n && isSet) {
            end = i;
            break;
        }


        i += cSize;
        j++;
    }

    size_t amount = end - start;
    return str.substr(start, amount);
}

void erase(std::string& str, int pos, int n) {
    size_t i = 0; // index relative to string bytes
    size_t j = 0; // index relative to "printable" unicode length

    bool isSet = false;
    int start = 0;
    int end = 0;

    while(i <= str.length()) {
        size_t cSize = codepointSize(str[i]);
        if (cSize == -1) {
            throw std::runtime_error("Invalid UTF-8 character");
        }

        if (j == pos) {
            isSet = true;
            start = i;
        }

        if (j == start + n && isSet) {
            end = i;
            break;
        }


        i += cSize;
        j++;
    }

    size_t amount = end - start;
    str.erase(start, amount);
}

// temporary shitty implementation
void appendChar(std::string& str, int pos, const char* chars) {
    size_t i = 0; // index relative to string bytes
    size_t j = 0; // index relative to "printable" unicode length

    while(i <= str.length()) {
        size_t cSize = codepointSize(str[i]);
        if (cSize == -1) {
            throw std::runtime_error("Invalid UTF-8 character");
        }

        if (j == pos) {
            str.insert(i, chars, codepointSize(chars[0]));
            break;
        }

        i += cSize;
        j++;
    }
}

namespace utf8 {
    int codepoint_size(char byte) {
        if ((byte & 0x80) == 0) // 0xxxxxxx
            return 1;
        else if ((byte & 0xE0) == 0xC0) // 110xxxxx
            return 2;
        else if ((byte & 0xF0) == 0xE0) // 1110xxxx
            return 3;
        else if ((byte & 0xF8) == 0xF0) // 11110xxx
            return 4;
        return 0;
    }
}
