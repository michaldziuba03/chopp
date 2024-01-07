#pragma once
#include <cstddef>
#include <unistd.h>
#include "vterminal.cc"

enum Keys {
    ENTER,
    ESCAPE,
    DELETE,
    TAB,
    BACKSPACE,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    CHAR,
    UNKNOWN,
};

Keys detectArrows(char key) {
        switch (key) {
            case 'A':
                return ARROW_UP;
            case 'B':
                return ARROW_DOWN;
            case 'C':
                return ARROW_RIGHT;
            case 'D':
                return ARROW_LEFT;
            default:
                return UNKNOWN;
    }
}

struct Event {
    Keys type;
    size_t len = 0;

    Event(Keys ntype) {
        type = ntype;
    }

    Event(Keys ntype, size_t nlen) {
        type = ntype;
        len = nlen;
    }
};

bool hasCTRL(char c, char expectedKey) {
    return ((expectedKey & 0x1f) == c);
}

Event poll(char buf[]) { 
    size_t readno = read(STDIN_FILENO, buf, 4);

    if (iscntrl(buf[0]) && readno > 0) {
        if (buf[0] == '\n') {
            return Event(ENTER, readno); 
        }

        if (buf[0] == '\t') {
            return Event(TAB, readno);
        }

        if (readno == 3 && buf[1] == '[' && buf[2] >= 'A' && buf[2] <= 'D' ) {
            return Event(detectArrows(buf[2]), readno);
        }

        if (readno == 4 && buf[1] == '[' && buf[2] == '3' && buf[3] == '~') {
            return Event(DELETE, readno);
        }

        if(buf[0] == 127) {
            return Event(BACKSPACE, readno);
        }
    } else if (readno > 0) {
        return Event(CHAR, readno);
    }

    return Event(UNKNOWN, readno);
}
