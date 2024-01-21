#pragma once

struct MouseEvent {
    int col = 0;
    int row = 0;
};

enum Keys {
    UNKNOWN = 0,
    ENTER,
    ESCAPE,
    DELETE,
    TAB,
    SPACE,
    BACKSPACE,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    MOUSE_LEFT,
    MOUSE_RIGHT,
    CHAR,
};

struct Event {
    Keys type;
    union {
        MouseEvent mouseEvent;
    };
    std::string chars;

    Event(Keys type) : type(type) {}
    Event(std::string chars) : type(CHAR), chars(chars) {}
};
