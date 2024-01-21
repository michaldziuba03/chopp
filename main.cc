#include <vector>
#include <iostream>
#include "terminal.cc"
#include "events.hh"
#include "utf8.cc"
#include "input.cc"
#include "fs.cc"

void cleanup() {
    terminal::disable_raw();
    terminal::leave_alternate_screen();
}

int main() {
    terminal::enable_raw();
    terminal::enter_alternate_screen();
    atexit(cleanup);
    Input input;
    terminal::move_to(1, 1);
    while (true) {
        terminal::foreground(RED);
        terminal::flush();
        input.poll();
        while(!input.empty()) {
            Event ev = input.next();
            if (ev.type == CHAR) {
                std::cout << ev.type << ": " << ev.chars << " len: " << ev.chars.length() << '\r' << std::endl;
                if (ev.chars == "q" || ev.chars == "Q") {
                    exit(0);
                }
            } else {
                std::cout << ev.type << '\r' << std::endl;
            }
        }
    }

    return 0;
}
