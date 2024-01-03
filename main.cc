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
                terminal::mvprintLn(1, i, "~");
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
            case ENTER:
                terminal::newLine();
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
                curX++;
                break;
            case ARROW_LEFT:
                curX--;
                break;
            case ARROW_UP:
                curY--;
                curX = 1;
                break;
            case ARROW_DOWN:
                curY++;
                curX = 1;
                break;
        }
    };

    return 0;
}
