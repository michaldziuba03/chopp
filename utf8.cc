#pragma once

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
