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
};

inline std::string backgroundANSI(Colors color) {
    return "\x1b[" + std::to_string(40 + color) + "m";
}

inline std::string foregroundANSI(Colors color) {
    return "\x1b[" + std::to_string(30 + color) + "m";
}
