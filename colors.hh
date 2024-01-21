#include <string>

#define C_RESET "\x1b[0m"

enum Colors {
    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    WHITE = 7,
    DEFAULT = 9,
};

struct RGB {
    int red = 0;
    int green = 0;
    int blue = 0;

    RGB(int red, int green, int blue) : red(red), green(green), blue(blue) {}
};

#define from_hex_str(x) std::stoi(x, nullptr, 16)

// Support formats: "#fff", "#ffffff", "fff", "ffffff"
RGB hex(const std::string& hex_color) {
    size_t hash_len = hex_color.at(0) == '#' ? 1 : 0;
    size_t len = hex_color.length() - hash_len;

    if (len != 6 && len != 3) {
        return RGB(0, 0, 0);
    }

    if (len == 3) { // support shorthand like: #fff
         std::string r_str = hex_color.substr(0 + hash_len, 1);
         std::string g_str = hex_color.substr(1 + hash_len, 1);
         std::string b_str = hex_color.substr(2 + hash_len, 1);

        int red = from_hex_str(r_str + r_str);
        int green = from_hex_str(g_str + g_str);
        int blue = from_hex_str(b_str + b_str);

        return RGB(red, green, blue);
    }

    std::string r_str = hex_color.substr(0 + hash_len, 2);
    std::string g_str = hex_color.substr(2 + hash_len, 2);
    std::string b_str = hex_color.substr(4 + hash_len, 2);

    int red = from_hex_str(r_str);
    int green = from_hex_str(g_str);
    int blue = from_hex_str(b_str);

    return RGB(red, green, blue);
}

inline std::string backgroundRGB(RGB rgb) {
    return "\x1b[48;2;" + std::to_string(rgb.red) + ";" + std::to_string(rgb.green) + ";" + std::to_string(rgb.blue) + "m";
}

inline std::string backgroundANSI(Colors color) {
    return "\x1b[" + std::to_string(40 + color) + "m";
}

inline std::string foregroundRGB(RGB rgb) {
    return "\x1b[38;2;" + std::to_string(rgb.red) + ";" + std::to_string(rgb.green) + ";" + std::to_string(rgb.blue) + "m";
}

inline std::string foregroundANSI(Colors color) {
    return "\x1b[" + std::to_string(30 + color) + "m";
}
