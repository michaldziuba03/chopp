#include <optional>
#include <unistd.h>
#include <iostream>
#include "utf8.cc"
#include <string.h>
#include <string>
#include <vector>

enum KeyModifiers {
    CTRL = 1,
    SHIFT = 2,
};

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
        End,
        Home,
        Menu,
    };

    KeyType type = Unknown;
    int modifiers = 0;
    int codepoint = 0;

    Key() = default;
    constexpr Key(KeyType type) : type(type) {}
    constexpr Key(KeyType type, int modifiers) : type(type), modifiers(modifiers) {}

    constexpr Key(int codepoint) : type(Key::Char), codepoint(codepoint) {}
    constexpr Key(int codepoint, int modifiers) : type(Key::Char), codepoint(codepoint), modifiers(modifiers) {}

    auto operator==(Key key) const { return key.type == this->type; }
    auto operator!=(Key key) const { return key.type != this->type; }

    inline bool ctrl() const {
        return (modifiers & KeyModifiers::CTRL) != 0;
    }

    inline bool shift() const {
        return (modifiers & KeyModifiers::SHIFT) != 0;
    }

    inline std::string str() const {
        return utf8::from_codepoint(codepoint);
    }
};

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

class Input {
    std::optional<char> get_char();
    std::optional<Key> parse_chars(char);
    std::optional<Key> parse_ansi();
    std::vector<Param> parse_params(std::optional<char>&);
public:
    std::optional<Key> get_next_key();
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

std::vector<Param> Input::parse_params(std::optional<char> &c) {
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

    return params;
}

std::optional<Key> Input::parse_ansi() {
    auto c = get_char(); // character after \x1b[ or \x1bO
    if (!c) return {};
    auto params = parse_params(c);

    switch (*c) {
        case 'A': return Key::Up;
        case 'B': return Key::Down;
        case 'C': return Key::Right;
        case 'D': return Key::Left;
        case 'F': return Key::End;
        case 'H': return Key::Home;
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
            case 29: return Key::Menu;
            }
    }

    return {};
}

std::optional<Key> Input::parse_chars(char c) {    
    if (c > 0 && c < 27) {
        return Key((c + 'a' - 1), KeyModifiers::CTRL);
    }

    if (c >= 27 && c < 32) {
        return Key((c + 'A' - 1), KeyModifiers::CTRL);
    }

    if (isupper(c)) {
        return Key(c, KeyModifiers::SHIFT);
    }

    size_t cp_size = utf8::codepoint_size(c);
    if (!cp_size) {
        return {};
    }

    char chars[5]{};
    chars[0] = c;
    for (int i = 1; i < cp_size; ++i) {
        chars[i] = get_char().value_or(0);
    }

    return Key(utf8::utf8_to_codepoint(chars));
}

std::optional<Key> Input::get_next_key() {
    auto c = get_char();
    if (!c) {
        return {};
    }
    //std::cout << int(*c) << "\r\n";
    switch (*c) {
        case 0: return Key(Key::Space, KeyModifiers::CTRL);
        case '\r': return Key::Return;
        case '\n': return Key::Return;
        case '\t': return Key::Tab;
        case '\b': return Key::Backspace;
        case 127: return Key::Backspace;
        case ' ': return Key::Space;
        case '\x1b':
            auto next = get_char(); 
            return (next && (*next == '[' || *next == 'O')) ? parse_ansi() : std::nullopt;
    }
    return parse_chars(*c);
}
