#pragma once
#include <sys/ioctl.h>
#include <termios.h>
#include <csignal>
#include <string>
#include "colors.hh"

inline void writeStdin(const char* buf, size_t len) {
    write(STDIN_FILENO, buf, len);
}

struct RawBuffer {
    std::string buf;

    inline void appendStr(const std::string& bytes) {
        buf.append(bytes);
    }

    inline void appendByte(char byte) {
        buf.push_back(byte);
    }

    void flush() {
        writeStdin(buf.c_str(), buf.length());
        buf.erase();
    }
};

namespace terminal {
    termios term{};
    size_t cols = 0;
    size_t rows = 0;
    RawBuffer buf;

    void disableRaw() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
    }

    void enableRaw() {
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

    inline void requestSize() {
        requestSize(0);
        std::signal(SIGWINCH, requestSize);
    }

    inline void enterAlternateScreen() {
        writeStdin("\x1b[?1049h", 9);
    }

    inline void leaveAlternateScreen() {
        writeStdin("\x1b[?1049l", 9);
    }

    inline void flush() {
        buf.flush();
    }

    inline void sleep(unsigned seconds) {
        usleep(seconds * 1000 * 1000); // takes microseconds
    }

    // methods to opearate in TUI:
    inline void moveTo(int x, int y) {
        buf.appendStr("\x1b[" + std::to_string(y) + ";" + std::to_string(x) + "H");
    }

    inline void print(const std::string& str) {
        buf.appendStr(str);
    }

    inline void printCh(const char byte) {
        buf.appendByte(byte);
    }

    inline void clearLine() {
        buf.appendStr("\x1b[K");
    }

    inline void newLine() {
        buf.appendStr("\r\n");
    }

    void mvprint(int x, int y, const std::string& str) {
        moveTo(x, y);
        clearLine();
        print(str);
    }

    void mvprintLn(int x, int y, const std::string& str) {
        mvprint(x, y, str);
        newLine();
    }

    void setForegroundColor(Colors color) {
        buf.appendStr(foregroundANSI(color));
    }

    void setForegroundColor(RGB rgb) {
        buf.appendStr(foregroundRGB(rgb));
    }

    void setBackgroundColor(Colors color) {
        buf.appendStr(backgroundANSI(color));
    }

    void setBackgroundColor(RGB rgb) {
        buf.appendStr(backgroundRGB(rgb));
    }

    void setBold() {
        buf.appendStr("\x1b[1m");
    }

    void resetStyles() {
        buf.appendStr(C_RESET);
    }
}
