#include "terminal.cc"
#include "input.cc"
#include <iostream>

void cleanup() {
    terminal::disable_raw();
    terminal::leave_alternate_screen();
}

int main() {
    terminal::enable_raw();
    terminal::enter_alternate_screen();
    atexit(cleanup);

    Input input;
    while (true) {
        auto event = input.get_next_key();
        if (event.has_value()) {
            auto key = event.value();
            std::cout << key.type << "\r" << std::endl;
            if (key == Key::Char) {
                if (key == "q" || key == "Q") {
                    exit(0);
                }

                std::cout << key.bytes << "\r" << std::endl;
            }
        }
        
    }
    
    return 0;
}
