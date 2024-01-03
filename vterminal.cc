#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <csignal>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>

struct RawBuffer {
    std::string buf;

    void appendStr(const std::string& bytes) {
        buf.append(bytes);
    }

    void appendByte(char byte) {
        buf.push_back(byte);
    }

    void flush() {
        write(STDIN_FILENO, buf.c_str(), buf.length());
        buf.erase();
    }
};

namespace terminal {
    termios term{};
    size_t cols;
    size_t rows;
    RawBuffer buf;

    void disableRaw()
    {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
    }

    void raw() {
        atexit(disableRaw);
        tcgetattr(STDIN_FILENO, &term);
        termios temp_term = term;
        temp_term.c_oflag &= ~(OPOST);
        temp_term.c_lflag &= ~(ECHO | ICANON);
        temp_term.c_cc[VMIN] = 0;
        temp_term.c_cc[VTIME] = 1;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &temp_term);
    }

    void requestSize(int _) {
        winsize windowSize{};
        int result = ioctl(STDIN_FILENO, TIOCGWINSZ, &windowSize);
        cols = windowSize.ws_col;
        rows = windowSize.ws_row;
    }

    void enterAlternateScreen() {
        buf.appendStr("\x1b[?1049h");
    }

    void leaveAlternateScreen() {
        buf.appendStr("\x1b[?1049l");
    }

    // methods to opearate in TUI:
    void moveTo(int x, int y) {
        buf.appendStr(std::to_string(y) + ";" + std::to_string(x) + "H");
    }

    void print(const std::string& str) {
        buf.appendStr(str);
    }

    void printCh(const char byte) {
        buf.appendByte(byte);
    }

    void clearLine() {
        buf.appendStr("\x1b[K");
    }
}
