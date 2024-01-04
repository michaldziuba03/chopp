#include <vector>
#include <iostream>
#include "vterminal.cc"
#include "events.cc"
#include "utf8.cc"

int main() {
    terminal::raw();
    terminal::requestSize();
    terminal::enterAlternateScreen();
    atexit(terminal::leaveAlternateScreen);

    int curX = 1, curY = 1;
    terminal::moveTo(curX, curY);
    std::vector<std::string> text;
    std::string curLine;
    text.emplace_back(curLine);

    while(true) {
        // Draw text lines
        for (int i = 1; i < terminal::rows; ++i)
        {
            if (text.size() < i) {
                terminal::setForegroundColor(BLUE);
                terminal::mvprintLn(1, i, "~");
                terminal::resetStyles();
            } else {
                terminal::mvprintLn(1, i, text[i - 1]);
            }
        }

        // Draw statusbar:
        terminal::setBackgroundColor(BLUE);
        terminal::setForegroundColor(WHITE);
        terminal::setBold();
        std::string label = "Lines: " + std::to_string(text.size());
        std::string statusSpace = std::string(terminal::cols - label.length(), ' ');

        std::string statusBar = label + statusSpace;
        terminal::mvprint(1, terminal::rows, statusBar);
        terminal::resetStyles();
        terminal::moveTo(curX, curY);
        terminal::flush();
        
        // process input:
        char chars[4] = {};
        auto event = poll(chars);

        switch (event.type) {
            case BACKSPACE:
                if (curX == 1 && curY > 1) {
                    curX = columnLen(text[curY - 2]) + 1;
                    text[curY - 2] += text[curY - 1];
                    text.erase(text.begin() + curY - 1);
                    curY--;
                } else if (curX > 0) {
                    removeChar(text[curY - 1], curX - 2);
                    curX--;
                }
                break;
            case ENTER:
                text.emplace_back("");
                curX = 1;
                curY++;
                break;
            case CHAR:
                if (chars[0] == 'q') {
                    return 0;
                }
                appendChar(text[curY - 1], curX - 1, chars);
                curX++;
                break;
            case ARROW_RIGHT:
                if (curX < columnLen(text[curY - 1]) + 1) {
                    curX++;
                }
                break;
            case ARROW_LEFT:
                if (curX > 1) {
                    curX--;
                }
                break;
            case ARROW_UP:
                if (curY > 1) {
                    curY--;
                    curX = 1;
                }
                break;
            case ARROW_DOWN:
                if (curY < text.size()) {
                    curY++;
                    curX = 1;
                }
                break;
        }
    };

    return 0;
}
