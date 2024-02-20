#include <optional>
#include <unistd.h>
#include <string>
#include <vector>
#include "utf8.cc"

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
    Codepoint codepoint = 0;
    int modifiers = 0;

    Key() = default;
    constexpr Key(KeyType type) : type(type) {}
    constexpr Key(KeyType type, int modifiers) : type(type), modifiers(modifiers) {}

    constexpr Key(Codepoint codepoint) : type(Key::Char), codepoint(codepoint) {}
    constexpr Key(Codepoint codepoint, int modifiers) : type(Key::Char), codepoint(codepoint), modifiers(modifiers) {}

    auto operator==(Key key) const { return key.type == type; }
    auto operator!=(Key key) const { return key.type != type; }
    void set_modifiers(int modifiers) { this->modifiers = modifiers; }

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
    size_t size() const { return subparams.size(); }
    int operator[](int i) const {
        return (size() > i) ? subparams[i] : 0; 
    }
};

struct Mouse {
    enum MouseType {
        Unknown,
        Left,
        Middle,
        Right,
        ScrollUp,
        ScrollDown,
    };

    MouseType type;
    int modifiers;
    int cx = 1, cy = 1;
    constexpr Mouse(MouseType type, int cx, int cy, int modifiers = 0) : type(type), cx(cx), cy(cy), modifiers(modifiers) {}

    inline bool ctrl() const {
        return (modifiers & KeyModifiers::CTRL) != 0;
    }

    inline bool shift() const {
        return (modifiers & KeyModifiers::SHIFT) != 0;
    }

    inline bool alt() const {
        return (modifiers & KeyModifiers::ALT) != 0;
    }
};

enum EventType {
    MouseEvent,
    KeyEvent,
};

struct Event {
    EventType type;
    union {
        Key key;
        Mouse mouse;
    };

    Event(Key key) : type(KeyEvent), key(key) {}
    Event(Key::KeyType type) : type(KeyEvent) { key = Key(type); }
    Event(Mouse mouse) : type(MouseEvent), mouse(mouse) {}
};

class Input {
    std::optional<Mouse> parse_sgr_mouse();
    std::optional<Mouse> parse_normal_mouse();
    std::optional<Mouse> parse_mouse(std::vector<Param>);
    std::optional<char> get_char();
    std::optional<Event> parse_chars(char);
    std::optional<Key> map_u_key(Codepoint codepoint);
    std::optional<Event> parse_csi();
    std::optional<Event> parse_ss3();
    std::vector<Param> parse_params(std::optional<char>&);
public:
    std::optional<Event> next_event();
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

static inline bool is_private_area(Codepoint codepoint) {
    return (codepoint >= 57344 && codepoint <= 63743);
}

std::optional<Key> Input::map_u_key(Codepoint codepoint) {
    switch(codepoint) {
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
    return is_private_area(codepoint) ? Key::Unknown : Key(codepoint);
}

int get_modifiers(std::vector<Param> params) {
    if (params.size() > 1) {
        return std::max(params[1][0] - 1, 0);
    }

    return 0;
}

std::optional<Mouse> Input::parse_mouse(std::vector<Param> params) {
    if (params.size() < 3) return {};

    int mask = params[0][0];
    int button_num = (mask & 0x3) | ((mask & 0xC0) >> 4);
    int cx = params[1][0];
    int cy = params[2][0];

    int modifiers = 0;
    if (mask & 0x4) {
        modifiers |= KeyModifiers::SHIFT;
    }

    if (mask & 0x8) {
        modifiers |= KeyModifiers::ALT;
    }

    if (mask & 0x10) {
        modifiers |= KeyModifiers::CTRL;
    }

    auto detect_type = [button_num]() -> std::optional<Mouse::MouseType> {
        switch (button_num) {
            case 0: return Mouse::Left;
            case 1: return Mouse::Middle;
            case 2: return Mouse::Right;
            case 4: return Mouse::ScrollUp;
            case 5: return Mouse::ScrollDown;
            default: return {};
        }
    };
    auto type = detect_type();
    if (!type) return {};

    return Mouse(*type, cx, cy, modifiers);
}

std::optional<Mouse> Input::parse_sgr_mouse() {
    auto c = get_char();
    if (!c) return {};
    auto params = parse_params(c);
    if (*c != 'M') return {};
    return parse_mouse(params);
}

std::optional<Mouse> Input::parse_normal_mouse() {
    auto c = get_char();
    if (!c) return {};
    auto params = parse_params(c);
    return parse_mouse(params);
}

std::optional<Event> Input::parse_ss3() {
    auto c = get_char();
    if (!c) return {};

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
    }
    return {};
}

std::optional<Event> Input::parse_csi() {
    auto c = get_char();
    if (!c) return {};
    switch (*c) {
        case '<': return parse_sgr_mouse();
        case 'M': return parse_normal_mouse();
    }

    auto params = parse_params(c);
    int modifiers = get_modifiers(params);

    if (*c == 'M') return parse_mouse(params);

    auto map_key = [&]() -> std::optional<Key>  {
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
            case 'u': return map_u_key(params[0][0]);
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
            default: return {};
        }
    };

    auto key = map_key();
    if (key && modifiers) {
        key->set_modifiers(modifiers);
    }

    return key;
}

std::optional<Event> Input::parse_chars(char c) {    
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
        chars[i] = get_char().value_or(255);
    }

    Codepoint codepoint = utf8::to_codepoint(chars);
    if (utf8::is_uppercase(codepoint)) {
        return Key(codepoint, KeyModifiers::SHIFT);
    }

    return Key(codepoint);
}

std::optional<Event> Input::next_event() {
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
            auto next = get_char().value_or(0);
            switch (next) {
                case '[': return parse_csi();
                case 'O': return parse_ss3();
                default: return {};
            }
    }
    return parse_chars(*c);
}
