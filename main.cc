#include "terminal.cc"
#include "input.cc"
#include "utf8.cc"
#include <iostream>

void cleanup() {
    terminal::disable_raw();
    terminal::leave_alternate_screen();
    //terminal::disable_mouse_tracking();
}

int main() {
    terminal::enable_raw();
    terminal::enter_alternate_screen();
    //terminal::enable_mouse_tracking();
    atexit(cleanup);

    Input input;
    while (true) {
        auto event = input.get_next_key();
        if (event.has_value()) {
            auto key = event.value();
            if (key.ctrl()) {
                std::cout << "^";
            }
            std::cout << "[" << key.type << "] ";
            if (key == Key::Char) {
                std::cout << utf8::from_codepoint(key.codepoint) << "(" << key.codepoint << ") ";

                if (key.codepoint == 'c' && key.ctrl()) {
                    std::cout << "\r" << std::endl;
                    exit(0);
                }
            }

            std::cout << "\r\n";
        }
        
    }
    
    return 0;
}
