#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <csignal>
#include <cstdlib>
#include <string>
#include <vector>

static termios term{};

void disableRaw()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

void enableRaw() {
    atexit(disableRaw);
    tcgetattr(STDIN_FILENO, &term);
    termios temp_term = term;
    temp_term.c_oflag &= ~(OPOST);
    temp_term.c_lflag &= ~(ECHO | ICANON);
    temp_term.c_cc[VMIN] = 0;
    temp_term.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &temp_term);
}

unsigned int cols;
unsigned int rows;

void requestSize(int _)
{
    winsize windowSize{};
    int result = ioctl(STDIN_FILENO, TIOCGWINSZ, &windowSize);
    cols = windowSize.ws_col;
    rows = windowSize.ws_row;
}

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

#define C_WHITE_BG "\x1b[47m"
#define C_BLUE_BG "\x1b[44m"
#define C_BLACK_FG "\x1b[30m"
#define C_WHITE_FG "\x1b[37m"
#define C_RESET "\x1b[0m"

#define ANSI(x) "\x1b[" x

struct Commands {
    RawBuffer buf;

    void moveTo(int x, int y) {
        buf.appendStr("\x1b[" + std::to_string(y) + ";" + std::to_string(x) + "H");
    }

    void toggleAlternativeScreen() {
        buf.appendStr(ANSI("?1049h"));
    }

    void toggleCursor() {
        buf.appendStr("\x1b[?25l\x1b[H");
    }

    void print(const std::string& str) {
        buf.appendStr(str);
    }

    void printCh(const char byte) {
        buf.appendByte(byte);
    }

    void setWhiteBg() {
        buf.appendStr(C_WHITE_BG);
    }

    void setBlueBg() {
        buf.appendStr(C_BLUE_BG);
    }

    void setBlackFg() {
        buf.appendStr(C_BLACK_FG);
    }

    void setWhiteFg() {
        buf.appendStr(C_WHITE_FG);
    }

    void reset() {
        buf.appendStr(C_RESET);
    }

    void newLine() {
        buf.appendStr("\r\n");
    }

    void flush() {
        buf.flush();
    }

    void clearLine() {
        buf.appendStr(ANSI("K"));
    }
};

int main() {
    enableRaw();
    requestSize(0);
    std::signal(SIGWINCH, requestSize);
    // current cursor position:
    int curX = 1, curY = 1;
    std::vector<std::string> text;
    std::string curLine;
    text.emplace_back(curLine);
    Commands commands;
    commands.toggleAlternativeScreen();

    char c;

    while(true) {
        // Draw text lines
        for (int i = 1; i < rows; ++i)
        {
            commands.moveTo(1, i);
            commands.clearLine();
            if (text.size() < i) {
                commands.print("~\r\n");
            } else {
                commands.print(text[i - 1] + "\r\n");
            }
        }

        // Draw statusbar:
        commands.moveTo(1, rows);
        commands.setBlueBg();
        commands.setWhiteFg();
        std::string label = "Lines: " + std::to_string(text.size());
        commands.print(label + std::string(cols - label.length(), ' '));
        commands.reset();

        commands.moveTo(curX, curY);

        size_t readno = read(STDIN_FILENO, &c, sizeof(c));
        if (iscntrl(c) && readno > 0) {
            if (c == '\n') {
                commands.newLine();
                text.emplace_back("");
                curX = 1;
                curY++;
            } else {
                // todo I guess
            }
        } else if(readno > 0) {
            if (c == 'q') {
                break;
            }
            text[curY - 1].push_back(c);
            curX++;
        }

        commands.flush();
    };

    commands.toggleAlternativeScreen();
    return 0;
}
