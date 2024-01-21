#pragma once
#include <string>
#include "events.hh"
#include "utf8.cc"

class KeyParser {
    std::string::iterator iter;
    std::string::iterator iter_end;

    bool peek() {
        if (iter == iter_end) {
            return false;
        }
        ++iter;
        return true;
    }

    inline bool next_is(char c) {
        if (!has_next()) return false;
        return *(iter+1) == c;
    }

    inline bool is_space() {
        return *iter == ' ';
    }

    inline bool is_backspace() {
        return *iter == '\b' || *iter == 127;
    }

    inline bool is_enter() {
        return *iter == '\n';
    }

    inline bool is_tab() {
        return *iter == '\t';
    }

    inline bool is_sequence() {
        return *iter == '\x1b' && next_is('[');
    }

    inline bool is_arrow() {
        return *iter >= 'A' && *iter <= 'D';
    }

    inline bool is_delete() {
        return *iter == '3' && next_is('~');
    }

    int is_char() {
        if (iscntrl(*iter)) {
            return 0;
        }

        return utf8::codepoint_size(*iter);
    }

public:
    KeyParser(std::string& raw_input) {
        iter = raw_input.begin();
        iter_end = raw_input.end();
    }

    inline bool has_next() {
        return iter != iter_end;
    }

    Event parse() {
        if (is_space()) {
            iter++;
            return Event(SPACE);
        }

        if (is_backspace()) {
            iter++;
            return Event(BACKSPACE);
        }

        if (is_enter()) {
            iter++;
            return Event(ENTER);
        }

        if (is_tab()) {
            iter++;
            return Event(TAB);
        }

        if (is_sequence()) {
            iter += 2;
            if (is_arrow()) {
                Keys arrow_key = detect_arrow();
                iter++;
                return Event(arrow_key);
            }

            if (is_delete()) {
                iter+=2;
                return Event(DELETE);
            }
        }

        int codepoint_len = is_char();
        if (codepoint_len) {
            std::string chars(iter, iter+codepoint_len);
            iter += codepoint_len;
            return Event(chars);
        }

        peek();
        return Event(UNKNOWN);
    }

    Keys detect_arrow() {
            switch (*iter) {
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
};
