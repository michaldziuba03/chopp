#include "vterminal.cc"

void moveCursor(char key, int& cx, int& cy) {
    switch (key) {
        case 'A':
            cy--;
            break;
        case 'B':
            cy++;
            break;
        case 'C':
            cx++;
            break;
        case 'D':
            cx--;
            break;
    }
}

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

    char c[3];

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
        std::string label = "Lines: " + std::to_string(text.size());
        std::string statusSpace = std::string(terminal::cols - label.length(), ' ');

        std::string statusBar = label + statusSpace;
        terminal::mvprint(1, terminal::rows, statusBar);
        terminal::resetColors();
        terminal::moveTo(curX, curY);
        terminal::flush();
        
        size_t readno = read(STDIN_FILENO, &c, sizeof(c));
        if (iscntrl(c[0]) && readno > 0) {
            if (c[0] == '\n') {
                terminal::newLine();
                text.emplace_back("");
                curX = 1;
                curY++;
            } else {
                if (readno == 3 && c[1] == '[') {
                    moveCursor(c[2], curX, curY);
                }
            }
        } else if(readno > 0) {
            if (c[0] == 'q') {
                break;
            }

            for (int i = 0; i < readno; ++i) {
                text[curY - 1].push_back(c[i]);
            }

            curX++;
        }    
    };

    return 0;
}
