#include <optional>
#include <unistd.h>
#include <string>
#include <vector>
#include "utf8.cc"

#define INVALID_PARAM -1

enum KeyModifiers {
    SHIFT = 1,
    ALT = 2,
    CTRL = 4,
    SUPER = 8,
    HYPER = 16,
    META = 32,
    CAPS_LOCK = 64,
    NUM_LOCK = 128,
};

struct Key {
    enum KeyType {
        Unknown = 0,
        Enter,
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
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25,
        F26,
        F27,
        F28,
        F29,
        F30,
        F31,
        F32,
        F33,
        F34,
        F35,
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

    auto operator==(Key key) const { return key.type == type; }
    auto operator!=(Key key) const { return key.type != type; }
    void set_modifier(int modifiers) { this->modifiers = modifiers; }

    inline bool ctrl() const {
        return (modifiers & KeyModifiers::CTRL) != 0;
    }

    inline bool shift() const {
        return (modifiers & KeyModifiers::SHIFT) != 0;
    }

    inline bool alt() const {
        return (modifiers & KeyModifiers::ALT) != 0;
    }

    inline std::string str() const {
        return utf8::from_codepoint(codepoint);
    }
};

struct Param {
private:
    int param = 0;
    std::vector<int> subparams;
public:
    void append_digit(unsigned char c) {
        param = (c - '0') + param * 10;
    }
    void add_subparam() {
        subparams.push_back(param);
        param = 0;
    }
    int operator[](int i) const {
        return subparams.empty() ? INVALID_PARAM : subparams[i]; 
    }
};

class Input {
    std::optional<char> get_char();
    std::optional<Key> parse_chars(char);
    std::optional<Key> parse_u_key(std::vector<Param>);
    std::optional<Key> parse_ansi();
    std::vector<Param> parse_params(std::optional<char>&);
public:
    std::optional<Key> get_next_key();
};

std::optional<char> Input::get_char() {
    if (unsigned char c = 0; read(STDIN_FILENO, &c, 1) == 1) {
        return c;
    }

    return {};
}

std::vector<Param> Input::parse_params(std::optional<char> &c) {
    std::vector<Param> params;
    Param param;

    while (c) {
        if (isdigit(*c))
            param.append_digit(*c);
        else if (*c == ':')
            param.add_subparam();
        else if (*c == ';') {
            param.add_subparam();
            params.push_back(param);
            param = {};
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

inline bool is_private_area(int codepoint) {
    return (codepoint >= 57344 && codepoint <= 63743) ? true : false;
}

struct KeyboardProtocol {
    int codepoint = INVALID_PARAM;
    int modifier_mask = 0;

    KeyboardProtocol(const std::vector<Param> &params) {
        codepoint = params[0][0];
        if (params.size() > 1) {
            modifier_mask = params[1][0] - 1;
        }
    }
};

std::optional<Key> Input::parse_u_key(std::vector<Param> params) {
    KeyboardProtocol k(params);
    const int modifier_mask = std::max(k.modifier_mask, 0);
    if (k.codepoint == INVALID_PARAM) {
        return {};
    }

    auto detect_key = [k]() -> Key {
        switch(k.codepoint) {
            case '\x1b': return Key::Escape;
            case '\r': return Key::Enter;
            case '\n': return Key::Enter;
            case '\t': return Key::Tab;
            case '\b': return Key::Backspace;
            case 127: return Key::Backspace;
            case ' ': return Key::Space;
            case 57399: return Key('0');
            case 57400: return Key('1');
            case 57401: return Key('2');
            case 57402: return Key('3');
            case 57403: return Key('4');
            case 57404: return Key('5');
            case 57405: return Key('6');
            case 57406: return Key('7');
            case 57407: return Key('8');
            case 57408: return Key('9');
            case 57409: return Key('.');
            case 57410: return Key('/');
            case 57411: return Key('*');
            case 57412: return Key('-');
            case 57413: return Key('+');
            case 57414: return Key::Enter;
            case 57415: return Key('=');
            case 57416: return Key(',');
            case 57417: return Key::Left;
            case 57418: return Key::Right;
            case 57419: return Key::Up;
            case 57420: return Key::Down;
            case 57421: return Key::PageUp;
            case 57422: return Key::PageDown;
            case 57423: return Key::Home;
            case 57424: return Key::End;
            case 57425: return Key::Insert;
            case 57426: return Key::Delete;
            case 57376: return Key::F13;
            case 57377: return Key::F14;
            case 57378: return Key::F15;
            case 57379: return Key::F16;
            case 57380: return Key::F17;
            case 57381: return Key::F18;
            case 57382: return Key::F19;
            case 57383: return Key::F20;
            case 57384: return Key::F21;
            case 57385: return Key::F22;
            case 57386: return Key::F23;
            case 57387: return Key::F24;
            case 57388: return Key::F25;
            case 57389: return Key::F26;
            case 57390: return Key::F27;
            case 57391: return Key::F28;
            case 57392: return Key::F29;
            case 57393: return Key::F30;
            case 57394: return Key::F31;
            case 57395: return Key::F32;
            case 57396: return Key::F33;
            case 57397: return Key::F34;
            case 57398: return Key::F35;
        }
        if (is_private_area(k.codepoint)) return Key::Unknown;
        return Key(k.codepoint);
    };

    Key key = detect_key();
    key.set_modifier(modifier_mask);
    return key;
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
        case 'Z': return Key(Key::Tab, KeyModifiers::SHIFT);
        case 'u': return parse_u_key(params);
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
            case 23: return Key::F11;
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

    size_t cp_size = utf8::codepoint_size(c);
    if (!cp_size) {
        return {};
    }

    char chars[5]{};
    chars[0] = c;
    for (int i = 1; i < cp_size; ++i) {
        chars[i] = get_char().value_or(0);
    }

    int codepoint = utf8::to_codepoint(chars);
    if (utf8::is_uppercase(codepoint)) {
        return Key(codepoint, KeyModifiers::SHIFT);
    }

    return Key(codepoint);
}

std::optional<Key> Input::get_next_key() {
    auto c = get_char();
    if (!c) {
        return {};
    }
    switch (*c) {
        case 0: return Key(Key::Space, KeyModifiers::CTRL);
        case '\r': return Key::Enter;
        case '\n': return Key::Enter;
        case '\t': return Key::Tab;
        case '\b': return Key::Backspace;
        case 127: return Key::Backspace;
        case ' ': return Key::Space;
        case '\x1b':
            auto next = get_char().value_or(-1);
            switch (next) {
                case '[': return parse_ansi();
                case 'O': return parse_ansi();
            }
            return {};
    }
    return parse_chars(*c);
}
