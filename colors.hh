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
    int red = 255;
    int green = 255;
    int blue = 255;
    RGB(int nred, int ngreen, int nblue) {
        red = nred;
        green = ngreen;
        blue = nblue;
    }
};

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
