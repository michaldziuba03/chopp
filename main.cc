#include <locale>
#include <iostream>
#include "terminal.cc"
#include "textbuffer.cc"
#include "input.cc"
#include "utf8.cc"

void cleanup() {
    terminal::disable_raw();
    terminal::disable_kitty_protocol();
    terminal::leave_alternate_screen();
    terminal::disable_mouse_tracking();
}

void detect_key(const Key &key) {
    if (key.ctrl()) {
        std::cout << "^";
    }
    if (key.shift()) {
        std::cout << "⇧";
    }

    if (key.alt()) {
        std::cout << "?";
    }

    std::cout << "[" << key.type << "] ";
    if (key == Key::Char) {
        std::cout << key.str() << "(" << key.codepoint << ") ";

        if (key.codepoint == 'c' && key.ctrl()) {
            std::cout << "\r" << std::endl;
            exit(0);
        }
    }
    std::cout << "\r\n";
}

void detect_mouse(const Mouse &mouse) {
    if (mouse.ctrl()) {
        std::cout << "^";
    }
    if (mouse.shift()) {
        std::cout << "⇧";
    }

    if (mouse.alt()) {
        std::cout << "?";
    }
    
    switch (mouse.type) {
        case Mouse::Left: std::cout << "[LEFT]"; break;
        case Mouse::Middle: std::cout << "[MIDDLE]"; break;
        case Mouse::Right: std::cout << "[RIGHT]"; break;
        case Mouse::ScrollUp:
        case Mouse::ScrollDown:
            std::cout << "[SCROLL]"; break;
    }

    std::cout << " cx(" << mouse.cx << "); cy(" << mouse.cy << ");\r\n";
}

int main() {
    std::setlocale(LC_ALL, "C.utf8");
    terminal::set_title("Test - Chopp");
    terminal::enable_raw();
    terminal::enter_alternate_screen();
    terminal::use_kitty_protocol();
    terminal::enable_mouse_tracking();
    atexit(cleanup);

    Input input;
    while (true) {
        auto event = input.next_event();
        if (event) {
            switch (event->type) {
                case KeyEvent: detect_key(event->key); break;
                case MouseEvent: detect_mouse(event->mouse); break;
            }
        }
    }
    return 0;
}
