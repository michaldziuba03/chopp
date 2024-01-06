#include <vector>
#include <iostream>
#include "vterminal.cc"
#include "events.cc"
#include "utf8.cc"
#define TAB_SIZE 4

void cleanup() {
    terminal::disableRaw();
    terminal::leaveAlternateScreen();
}

int main() {
    terminal::enableRaw();
    terminal::requestSize();
    terminal::enterAlternateScreen();
    atexit(cleanup);

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
                if (i == curY) {
                    //terminal::setForegroundColor(RGB(255, 0, 0));
                    //terminal::setBackgroundColor(WHITE);
                    terminal::setBackgroundColor(RGB(42, 43, 60));
                    terminal::mvprintLn(1, i, text[i - 1]);
                    terminal::resetStyles();
                } else {
                    terminal::mvprintLn(1, i, text[i - 1]);
                }

            }
        }

        // Draw statusbar:
        terminal::setBackgroundColor(BLUE);
        //terminal::setBackgroundColor(hex("#E58C8A"));
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
            case TAB:
                // temporary solution for tabs:
                for (int i = 0; i < TAB_SIZE; ++i) {
                    appendChar(text[curY - 1], curX - 1, " ");
                    ++curX;
                }
                break;
            case BACKSPACE:
                if (curX == 1 && curY > 1) {
                    curX = columnLen(text[curY - 2]) + 1;
                    text[curY - 2] += text[curY - 1];
                    text.erase(text.begin() + curY - 1);
                    curY--;
                } else if (curX > 1) {
                    removeChar(text[curY - 1], curX - 2);
                    curX--;
                }
                break;
            case ENTER:
                if (curX <= columnLen(text[curY - 1])) {
                    text.insert(text.begin() + curY, substr(text[curY - 1], curX - 1, columnLen(text[curY - 1]) - curX + 1));
                    erase(text[curY - 1], curX - 1, columnLen(text[curY - 1]) - curX + 1);
                } else {
                    text.insert(text.begin() + curY, "");
                }
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
                if (curX <= columnLen(text[curY - 1])) {
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
                }

                if (curX >= columnLen(text[curY - 1])) {
                    curX = columnLen(text[curY - 1]) + 1;
                }
                break;
            case ARROW_DOWN:
                if (curY < text.size()) {
                    curY++;
                }

                if (curX >= columnLen(text[curY - 1])) {
                    curX = columnLen(text[curY - 1]) + 1;
                }
                break;
        }
    };

    return 0;
}
