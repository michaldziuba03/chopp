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
public:
    int red = 0;
    int green = 0;
    int blue = 0;

    RGB(int nred, int ngreen, int nblue) {
        red = nred;
        green = ngreen;
        blue = nblue;
    }
};

#define fromHexStr(x) std::stoi(x, nullptr, 16)

// Support formats: "#fff", "#ffffff", "fff", "ffffff"
RGB hex(const std::string& hexColor) {
    size_t hashLen = hexColor.at(0) == '#' ? 1 : 0;
    size_t len = hexColor.length() - hashLen;

    if (len != 6 && len != 3) {
        return RGB(0, 0, 0);
    }

    if (len == 3) { // support shorthand like: #fff
         std::string rStr = hexColor.substr(0 + hashLen, 1);
         std::string gStr = hexColor.substr(1 + hashLen, 1);
         std::string bStr = hexColor.substr(2 + hashLen, 1);

        int red = fromHexStr(rStr + rStr);
        int green = fromHexStr(gStr + gStr);
        int blue = fromHexStr(bStr + bStr);

        return RGB(red, green, blue);
    }

    std::string rStr = hexColor.substr(0 + hashLen, 2);
    std::string gStr = hexColor.substr(2 + hashLen, 2);
    std::string bStr = hexColor.substr(4 + hashLen, 2);

    int red = fromHexStr(rStr);
    int green = fromHexStr(gStr);
    int blue = fromHexStr(bStr);

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
