#include <sys/ioctl.h>
#include <termios.h>
#include <csignal>
#include <string>
#include "colors.hh"
#include <cstring>

namespace terminal {
    termios term{};
    size_t cols = 0;
    size_t rows = 0;
    std::string buf;

    inline void write_stdin(const std::string& str) {
        write(STDIN_FILENO, str.c_str(), str.length());
    }

    inline void append_str(const std::string& bytes) {
        buf.append(bytes);
    }

    inline void append_byte(char byte) {
        buf.push_back(byte);
    }

    void flush() {
        write_stdin(buf);
        buf.erase();
    }

    void enable_raw() {
        tcgetattr(STDIN_FILENO, &term);
        termios temp_term = term;
        temp_term.c_oflag &= ~(OPOST);
        temp_term.c_lflag &= ~(ECHO | ICANON | ISIG);
        temp_term.c_iflag &= ~(IXON);
        temp_term.c_cc[VMIN] = 0;
        temp_term.c_cc[VTIME] = 1;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &temp_term);
    }

    void disable_raw() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
    }

    void enter_alternate_screen() {
        write_stdin("\x1b[?1049h");
    }

    void leave_alternate_screen() {
        write_stdin("\x1b[?1049l");
    }

    // drawing
    void move_to(int x, int y) {
        append_str("\x1b[" + std::to_string(y) + ";" + std::to_string(x) + "H");
    }

    void print(const std::string& str) {
        append_str(str);
    }

    void printch(const char c) {
        append_byte(c);
    }

    void clearln() {
        append_str("\x1b[K");
    }

    void newline() {
        append_str("\r\n");
    }

    void mvprint(int x, int y, const std::string& str) {
        move_to(x, y);
        clearln();
        print(str);
    }

    void mvprintLn(int x, int y, const std::string& str) {
        mvprint(x, y, str);
        newline();
    }

    // styles:
    void reset() {
        append_str("\x1b[0m");
    }

    void bold() {
        append_str("\x1b[1m");
    }

    void underline() {
        append_str("\x1b[4m");
    }

    void reverse() {
        append_str("\x1b[7m");
    }

    void foreground(Colors color) {
        append_str(foregroundANSI(color));
    }

    void foreground(RGB rgb) {
        append_str(foregroundRGB(rgb));
    }

    void background(Colors color) {
        append_str(backgroundANSI(color));
    }

    void background(RGB rgb) {
        append_str(backgroundRGB(rgb));
    }
}
