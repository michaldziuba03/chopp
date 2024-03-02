#pragma once
#include <string>

typedef int32_t Codepoint;

namespace utf8 {
    Codepoint codepoint_size(unsigned char byte) {
        if ((byte < 0x80))
            return 1;
        else if ((byte & 0xE0) == 0xC0)
            return 2;
        else if ((byte & 0xF0) == 0xE0)
            return 3;
        else if ((byte & 0xF8) == 0xF0)
            return 4;
        return 0;
    }

    Codepoint to_codepoint(const char *utf8) {
        unsigned char *bytes = (unsigned char *)utf8;
        
        if (bytes[0] < 0x80) {
            return bytes[0];
        } else if ((bytes[0] & 0xE0) == 0xC0) {
            return ((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F);
        } else if ((bytes[0] & 0xF0) == 0xE0) {
            return ((bytes[0] & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) | (bytes[2] & 0x3F);
        } else if ((bytes[0] & 0xF8) == 0xF0) {
            return ((bytes[0] & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) | ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F);
        }

        return -1;
    }

    std::string from_codepoint(Codepoint codepoint) {
        char utf8[5]{};

        if (codepoint < 0x80) {
            utf8[0] = codepoint;
        } else if (codepoint < 0x800) {
            utf8[0] = 0xC0 | (codepoint >> 6);
            utf8[1] = 0x80 | (codepoint & 0x3F);
        } else if (codepoint < 0x10000) {
            utf8[0] = 0xE0 | (codepoint >> 12);
            utf8[1] = 0x80 | ((codepoint >> 6) & 0x3F);
            utf8[2] = 0x80 | (codepoint & 0x3F);
        } else if (codepoint < 0x110000) {
            utf8[0] = 0xF0 | (codepoint >> 18);
            utf8[1] = 0x80 | ((codepoint >> 12) & 0x3F);
            utf8[2] = 0x80 | ((codepoint >> 6) & 0x3F);
            utf8[3] = 0x80 | (codepoint & 0x3F);
        }

        return utf8;
    }

    inline bool is_uppercase(Codepoint codepoint) {
        return std::iswupper(codepoint);
    }

    inline bool is_lowercase(Codepoint codepoint) {
        return std::iswlower(codepoint);
    }
}
