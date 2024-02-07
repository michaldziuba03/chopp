#include <optional>
#include <unistd.h>
#include <iostream>
#include "utf8.cc"
#include <string.h>
#include <vector>

struct Key {
    enum KeyType {
        Unknown = 0,
        Return,
        Escape,
        Delete,
        Tab,
        Space,
        Backspace,
        Up,
        Down,
        Left,
        Right,
        Char,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        PageUp,
        PageDown,
        Insert,
    };

    KeyType type = Unknown;
    int modifiers = 0;

    char bytes[5]{};
    size_t length = 0;

    Key() = default;
    constexpr Key(KeyType type) : type(type) {}

    Key(const char *bytes, size_t length) : type(Key::Char), length(length) {
        std::copy(bytes, bytes+length, this->bytes);
    }

    auto operator==(const char *c) const { return strcmp(c, bytes) == 0; }
    auto operator!=(const char *c) const { return strcmp(c, bytes) != 0; }
    auto operator==(Key key) const { return key.type == this->type; }
    auto operator!=(Key key) const { return key.type != this->type; }
};

class Input {
public:
    std::optional<char> get_char();
    std::optional<Key> get_next_key();
    std::optional<Key> parse_chars(char);
    std::optional<Key> parse_cntrl(char);
    std::optional<Key> parse_ansi();
};

std::optional<char> Input::get_char() {
    if (char c = 0; read(STDIN_FILENO, &c, 1) == 1) {
        return c;
    }

    return {};
}

inline bool is_cntrl(const char c) {
    return iscntrl(c) || c == ' ';
}

inline int to_digit(char c) {
    return c - '0';
}

struct Param {
private:
    int param = 0;
    std::vector<int> subparams;
public:
    void append_digit(char c) {
        param = to_digit(c) + param * 10;
    }
    void add_subparam() {
        subparams.push_back(param);
        param = 0;
    }
    int operator[](int i) const {
        return subparams.empty() ? -1 : subparams[i]; 
    }
};

std::optional<Key> Input::parse_ansi() {
    auto c = get_char();
    if (!c) return {};

    std::vector<Param> params;
    Param param;

    while (c) {
        if (isdigit(*c)) {
            param.append_digit(*c);
        }
        else if (*c == ';') {
            params.push_back(param);
            param = {};
        }
        else if (*c == ':') {
            param.add_subparam();
        }
        else {
            param.add_subparam();
            params.push_back(param);
            param = {};
            break;
        }

        c = get_char();
    }

    switch (*c) {
        case 'A': return Key::Up;
        case 'B': return Key::Down;
        case 'C': return Key::Right;
        case 'D': return Key::Left;
        case 'P': return Key::F1;
        case 'Q': return Key::F2;
        case 'R': return Key::F3;
        case 'S': return Key::F4;
        case '~':
            switch (params[0][0]) 
            {
            case 2: return Key::Insert;
            case 3: return Key::Delete;
            case 5: return Key::PageUp;
            case 6: return Key::PageDown;
            case 15: return Key::F5;
            case 17: return Key::F6;
            case 18: return Key::F7;
            case 19: return Key::F8;
            case 20: return Key::F9;
            case 21: return Key::F10;
            case 24: return Key::F12;
            }
    }

    return {};
}

std::optional<Key> Input::parse_cntrl(char c) {
    switch (c) {
        case '\n': return Key::Return;
        case '\r': return Key::Return;
        case '\t': return Key::Tab;
        case '\b': return Key::Backspace;
        case 127: return Key::Backspace;
        case ' ': return Key::Space;
    }

    if (c == '\x1b') {
        if (auto next = get_char(); next && *next == '[' || *next == 'O')
            return parse_ansi();
    }

    return {};
}

std::optional<Key> Input::parse_chars(char c) {
    size_t cp_size = utf8::codepoint_size(c);
    if (!cp_size) {
        return {};
    }

    char *chars = new char[cp_size];
    chars[0] = c;
    for (int i = 1; i < cp_size; ++i) {
        chars[i] = get_char().value_or(0);
    }

    Key key(chars, cp_size);
    delete[] chars;

    return key;
}

std::optional<Key> Input::get_next_key() {
    auto c = get_char();
    if (!c) {
        return {};
    }
    if (is_cntrl(*c)) {
        return parse_cntrl(*c);
    }

    return parse_chars(*c);
}
